#include "JITRotation.h"

#include "Actor.h"
#include "Action.h"
#include "Model.h"

#include "compiler/Preprocessor.h"
#include "compiler/Parser.h"
#include "compiler/LLVMCodeGenerator.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>

bool JITRotation::initializeWithFile(const char* filename) {
	const char header[] =
		"class Actor;\n"
		"uint64 AuraCount(const Actor* actor, const uint8* identifier, const Actor* source);\n"
		"double GlobalCooldownRemaining(const Actor* actor);\n"
		"double CooldownRemaining(const Actor* actor, const uint8* identifier);\n"
		"double AuraTimeRemaining(const Actor* actor, const uint8* identifier, const Actor* source);\n"
		"Actor* Pet(Actor* owner);\n"
		"uint64 TP(const Actor* actor);\n"
		"uint64 MP(const Actor* actor);\n"
		"double GlobalCooldown(const Actor* actor);\n"
		"double Time(const Actor* actor);\n"
		"void RemoveAura(Actor* actor, const uint8* identifier, const Actor* source);\n"
		"uint8 IsReady(const Actor* actor, const uint8* identifier);\n"
		"void Command(Actor* actor, const uint8* identifier);\n"
		"void StopAutoAttack(Actor* actor);\n"
		"__end __hidden const uint8* NextAction(Actor* self, const Actor* target) {\n"
	;
	
	const char footer[] = "\n}";

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
		free(source);
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

	engine->addGlobalMapping(module->getFunction("^AuraCount"), (void*)&JITRotation::ActorAuraCount);
	engine->addGlobalMapping(module->getFunction("^GlobalCooldownRemaining"), (void*)&JITRotation::ActorGlobalCooldownRemaining);
	engine->addGlobalMapping(module->getFunction("^CooldownRemaining"), (void*)&JITRotation::ActorCooldownRemaining);
	engine->addGlobalMapping(module->getFunction("^AuraTimeRemaining"), (void*)&JITRotation::ActorAuraTimeRemaining);
	engine->addGlobalMapping(module->getFunction("^Pet"), (void*)&JITRotation::ActorPet);
	engine->addGlobalMapping(module->getFunction("^TP"), (void*)&JITRotation::ActorTP);
	engine->addGlobalMapping(module->getFunction("^MP"), (void*)&JITRotation::ActorMP);
	engine->addGlobalMapping(module->getFunction("^GlobalCooldown"), (void*)&JITRotation::ActorGlobalCooldown);
	engine->addGlobalMapping(module->getFunction("^Time"), (void*)&JITRotation::ActorTime);
	engine->addGlobalMapping(module->getFunction("^RemoveAura"), (void*)&JITRotation::ActorRemoveAura);
	engine->addGlobalMapping(module->getFunction("^IsReady"), (void*)&JITRotation::ActionIsReady);
	engine->addGlobalMapping(module->getFunction("^Command"), (void*)&JITRotation::ActorCommand);
	engine->addGlobalMapping(module->getFunction("^StopAutoAttack"), (void*)&JITRotation::ActorStopAutoAttack);

	_jitNextAction = decltype(_jitNextAction)((std::intptr_t)engine->getPointerToFunction(module->getFunction("^NextAction")));

	return _jitNextAction;
}

const Action* JITRotation::nextAction(Actor* subject, const Actor* target) const {
	auto identifier = _jitNextAction(subject, target);
	return identifier ? subject->model()->action(identifier) : nullptr;
}

uint64_t JITRotation::ActorAuraCount(const Actor* actor, const char* identifier, const Actor* source) {
	return actor->auraCount(identifier, source);
}

double JITRotation::ActorGlobalCooldownRemaining(const Actor* actor) {
	return std::chrono::duration<double>(actor->globalCooldownRemaining()).count();
}

double JITRotation::ActorCooldownRemaining(const Actor* actor, const char* identifier) {
	return std::chrono::duration<double>(actor->cooldownRemaining(identifier)).count();
}

double JITRotation::ActorAuraTimeRemaining(const Actor* actor, const char* identifier, const Actor* source) {
	return std::chrono::duration<double>(actor->auraTimeRemaining(identifier, source)).count();
}

Actor* JITRotation::ActorPet(Actor* actor) {
	return actor->pet();
}

uint64_t JITRotation::ActorTP(const Actor* actor) {
	return actor->tp();
}

uint64_t JITRotation::ActorMP(const Actor* actor) {
	return actor->mp();
}

double JITRotation::ActorGlobalCooldown(const Actor* actor) {
	return std::chrono::duration<double>(actor->globalCooldown()).count();
}

double JITRotation::ActorTime(const Actor* actor) {
	return std::chrono::duration<double>(actor->time()).count();
}

void JITRotation::ActorRemoveAura(Actor* actor, const char* identifier, const Actor* source) {
	actor->dispelAura(identifier, source, INT_MAX);
}

uint8_t JITRotation::ActionIsReady(const Actor* actor, const char* identifier) {
	return actor->model()->action(identifier)->isReady(actor);
}

void JITRotation::ActorCommand(Actor* actor, const char* identifier) {
	actor->setCommand(actor->model()->action(identifier));
}

void JITRotation::ActorStopAutoAttack(Actor* actor) {
	actor->stopAutoAttack();
}
