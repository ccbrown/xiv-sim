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
	// PREPROCESS

	Preprocessor pp;

	if (!pp.process_file(filename)) {
		printf("Couldn't preprocess file.\n");
		return false;
	}
	
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

	_jitSubject = decltype(_jitSubject)(engine->getPointerToGlobal(module->getGlobalVariable("^_Subject")));
	_jitTarget = decltype(_jitTarget)(engine->getPointerToGlobal(module->getGlobalVariable("^_Target")));
	_jitNextAction = decltype(_jitNextAction)((std::intptr_t)engine->getPointerToFunction(module->getFunction("^NextAction")));

	return _jitNextAction;
}

const Action* JITRotation::nextAction(const Actor* subject, const Actor* target) const {
	*_jitSubject = subject;
	*_jitTarget = target;
	auto identifier = _jitNextAction();
	return identifier ? subject->model()->action(identifier) : nullptr;
}
