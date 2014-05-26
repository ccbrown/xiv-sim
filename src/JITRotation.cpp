#include "JITRotation.h"

#include "Actor.h"
#include "Model.h"

#include "compiler/Preprocessor.h"
#include "compiler/Parser.h"
#include "compiler/LLVMCodeGenerator.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>

extern "C" {
	uint64_t JITRotationActorAuraCount(const Actor* actor, const char* identifier) {
		return actor->auraCount(identifier);
	}
	double JITRotationActorCooldownRemaining(const Actor* actor, const char* identifier) {
		return std::chrono::duration<double>(actor->cooldownRemaining(identifier)).count();
	}
	double JITRotationActorAuraTimeRemaining(const Actor* actor, const char* identifier) {
		return std::chrono::duration<double>(actor->auraTimeRemaining(identifier)).count();
	}
	uint64_t JITRotationActorTP(const Actor* actor) {
		return actor->tp();
	}
}

bool JITRotation::initializeWithFile(const char* filename) {
	const char header[] =
		"class Actor;"
		"extern uint64 AuraCount(const Actor* actor, const uint8* identifier) : \"JITRotationActorAuraCount\";"
		"extern double CooldownRemaining(const Actor* actor, const uint8* identifier) : \"JITRotationActorCooldownRemaining\";"
		"extern double AuraTimeRemaining(const Actor* actor, const uint8* identifier) : \"JITRotationActorAuraTimeRemaining\";"
		"extern uint64 TP(const Actor* actor) : \"JITRotationActorTP\";"
		"hidden const uint8* NextAction(const Actor* self, const Actor* target) {"
	;
	
	const char footer[] = "}";

	FILE* f = fopen(filename, "r");

	if (!f) {
		printf("Unable to open file %s\n", filename);
		return false;
	}

	fseek(f, 0, SEEK_END);
	auto fsize = ftell(f);
	rewind(f);
 
 	auto sourceLength = sizeof(header) + fsize + sizeof(footer);
 
	auto source = (char*)malloc(sourceLength);
	if (!source) {
		printf("Unable to allocate memory for rotation source\n");
		fclose(f);
		return false;
	}

	memcpy(source, header, sizeof(header));

	if (fread(source + sizeof(header), fsize, 1, f) != 1) {
		printf("Unable to read file %s\n", filename);
		fclose(f);
		return false;
	}

	memcpy(source + sizeof(header) + fsize, footer, sizeof(footer));

	fclose(f);

	// PREPROCESS

	Preprocessor pp;

	if (!pp.process_file(filename, source, sourceLength)) {
		printf("Couldn't preprocess file.\n");
		return false;
	}

	free(source);
	
	// PARSE
	
	Parser p;
	
	ASTSequence* ast = p.generate_ast(pp.tokens());
	
	if (p.errors().size() > 0) {
		delete ast;
		for (const ParseError& e : p.errors()) {
			printf("Error: %s\n", e.message.c_str());
			e.token->print_pointer();
		}
		return false;
	}
	
	if (!ast) {
		printf("Couldn't generate AST.\n");
		return false;
	}

	// GENERATE CODE

	LLVMCodeGenerator cg;

	if (!cg.build_ir(ast)) {
		printf("Couldn't build IR.\n");
		delete ast;
		return false;
	}

	delete ast;

	// COMPILE

	LLVMInitializeNativeTarget();
	
	auto module = cg.release_module();

	std::string error;
	auto engine = llvm::EngineBuilder(module).setErrorStr(&error).setEngineKind(llvm::EngineKind::JIT).create();

	if (!engine) {
		printf("Couldn't build execution engine: %s\n", error.c_str());
		return false;
	}

	_jitNextAction = decltype(_jitNextAction)((std::intptr_t)engine->getPointerToFunction(module->getFunction("^NextAction")));

	return _jitNextAction;
}

const Action* JITRotation::nextAction(const Actor* subject, const Actor* target) const {
	auto identifier = _jitNextAction(subject, target);
	return identifier ? subject->model()->action(identifier) : nullptr;
}
