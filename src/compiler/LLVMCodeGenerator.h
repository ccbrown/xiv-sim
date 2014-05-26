#pragma once

#include "AST.h"

#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Analysis/Verifier.h>

class LLVMCodeGenerator : public ASTNodeVisitor {
	public:
		LLVMCodeGenerator();
		virtual ~LLVMCodeGenerator();
	
		bool build_ir(ASTNode* ast);
		
		llvm::Module* release_module();

		virtual const void* visit(ASTNode* node);
		virtual const void* visit(ASTNop* node);
		virtual const void* visit(ASTExpression* node);
		virtual const void* visit(ASTSequence* node);
		virtual const void* visit(ASTVariableRef* node);
		virtual const void* visit(ASTVariableDec* node);
		virtual const void* visit(ASTFunctionRef* node);
		virtual const void* visit(ASTFunctionProto* node);
		virtual const void* visit(ASTFunctionDef* node);
		virtual const void* visit(ASTStructMemberRef* node);
		virtual const void* visit(ASTFloatingPoint* node);
		virtual const void* visit(ASTInteger* node);
		virtual const void* visit(ASTConstantArray* node);
		virtual const void* visit(ASTUnaryOp* node);
		virtual const void* visit(ASTBinaryOp* node);
		virtual const void* visit(ASTReturn* node);
		virtual const void* visit(ASTInlineAsm* node);
		virtual const void* visit(ASTFunctionCall* node);
		virtual const void* visit(ASTCast* node);
		virtual const void* visit(ASTCondition* node);
		virtual const void* visit(ASTWhileLoop* node);
		virtual const void* visit(ASTNullPointer* node);
			
	private:
		llvm::Value* _value(ASTExpression* exp);
		llvm::Value* _dereferenced_value(ASTExpression* exp);
		llvm::Type* _llvm_type(SLTypePtr type);

		void _build_basic_block(llvm::BasicBlock* block, ASTNode* node, llvm::BasicBlock* next);
	
		llvm::LLVMContext& _context;
		llvm::Module* _module;
		llvm::IRBuilder<> _builder;

		struct FunctionContext {
			FunctionContext() = default;
			FunctionContext(SLFunctionPtr c3_function, llvm::Function* llvm_function, llvm::AllocaInst* return_alloca, llvm::BasicBlock* return_block)
				: c3_function(c3_function), llvm_function(llvm_function), return_alloca(return_alloca), return_block(return_block) {}
			
			SLFunctionPtr c3_function = nullptr;
			llvm::Function* llvm_function = nullptr;
			llvm::AllocaInst* return_alloca = nullptr;
			llvm::BasicBlock* return_block = nullptr;
		};
		
		FunctionContext _current_function_context;
		bool _is_current_block_terminated = false;

		std::unordered_map<std::string, llvm::Value*> _named_values;
		std::unordered_map<std::string, llvm::Type*> _named_types;
};
