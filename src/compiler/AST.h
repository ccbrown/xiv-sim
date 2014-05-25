#pragma once

#include "C3/C3.h"

#include <string>
#include <list>
#include <vector>
#include <cstdlib>

class ASTNodeVisitor;

struct ASTNode {
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTNode() {}
};

struct ASTNop : ASTNode {
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTNop() {}
};

struct ASTExpression : ASTNode {
	C3TypePtr type;
	bool is_constant = false;

	ASTExpression(C3TypePtr type, bool is_constant = false) : type(type), is_constant(is_constant) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTExpression() {}
};

struct ASTSequence : ASTNode {
	std::list<ASTNode*> sequence;
	
	ASTSequence() {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTSequence();
};

struct ASTVariableRef : ASTExpression {
	C3VariablePtr var;

	ASTVariableRef(C3VariablePtr var) : ASTExpression(C3Type::ReferenceType(var->type())), var(var) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTVariableRef() {}
};

struct ASTVariableDec : ASTNode {
	C3VariablePtr var;
	ASTExpression* init;

	ASTVariableDec(C3VariablePtr var, ASTExpression* init = nullptr) : var(var), init(init) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTVariableDec() { delete init; }
};

struct ASTFunctionRef : ASTExpression {
	C3FunctionPtr func;

	ASTFunctionRef(C3FunctionPtr func) : ASTExpression(func->type()), func(func) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTFunctionRef() {}
};

struct ASTFunctionProto : ASTNode {
	C3FunctionPtr func;
	std::vector<std::string> arg_names;

	ASTFunctionProto(C3FunctionPtr func, const std::vector<std::string>& arg_names) : func(func), arg_names(arg_names) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTFunctionProto() {}
};

struct ASTFunctionDef : ASTNode {
	ASTFunctionProto* proto;
	ASTSequence* body;
	std::string arg_prefix;

	ASTFunctionDef(ASTFunctionProto* proto, ASTSequence* body, const std::string& arg_prefix) : proto(proto), body(body), arg_prefix(arg_prefix) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTFunctionDef();
};

struct ASTStructMemberRef : ASTExpression {
	ASTExpression* structure;
	size_t index;

	ASTStructMemberRef(ASTExpression* structure, size_t index);
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTStructMemberRef();
};

struct ASTFloatingPoint : ASTExpression {
	double value;

	ASTFloatingPoint(double value, C3TypePtr type) : ASTExpression(type, true), value(value) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTFloatingPoint() {}
};

struct ASTInteger : ASTExpression {
	uint64_t value;

	ASTInteger(uint64_t value, C3TypePtr type) : ASTExpression(type, true), value(value) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTInteger() {}
};

struct ASTConstantArray : ASTExpression {
	void* data;
	size_t size;
	
	ASTConstantArray(const void* data, size_t size, C3TypePtr type) : ASTExpression(C3Type::PointerType(type), true) {
		this->data = malloc(size);
		memcpy(this->data, data, size);
		this->size = size;
	}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTConstantArray() {
		free(data);
	}
};

struct ASTUnaryOp : ASTExpression {
	std::string op;
	ASTExpression* right;
	C3TypePtr type;

	ASTUnaryOp(const std::string& op, ASTExpression* right, C3TypePtr type) : ASTExpression(type), op(op), right(right) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTUnaryOp();
};

struct ASTBinaryOp : ASTExpression {
	std::string op;
	ASTExpression* left;
	ASTExpression* right;
	C3TypePtr type;

	ASTBinaryOp(const std::string& op, ASTExpression* left, ASTExpression* right, C3TypePtr type) : ASTExpression(type), op(op), left(left), right(right) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTBinaryOp();
};

struct ASTReturn : ASTNode {
	ASTExpression* value;

	ASTReturn(ASTExpression* value) : value(value) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTReturn() { delete value; }
};

struct ASTInlineAsm : ASTNode {
	std::string assembly;
	std::vector<ASTExpression*> outputs;
	std::vector<ASTExpression*> inputs;
	std::vector<std::string> constraints;
	
	ASTInlineAsm(const std::string& assembly, std::vector<ASTExpression*>& outputs, std::vector<ASTExpression*>& inputs, std::vector<std::string>& constraints) : assembly(assembly), outputs(outputs), inputs(inputs), constraints(constraints) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTInlineAsm();
};

struct ASTFunctionCall : ASTExpression {
	ASTExpression* func;
	std::vector<ASTExpression*> args;

	ASTFunctionCall(ASTExpression* func, const std::vector<ASTExpression*>& args) : ASTExpression(func->type->signature().return_type()), func(func), args(args) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTFunctionCall();
};

struct ASTCast : ASTExpression {
	ASTExpression* original;

	ASTCast(ASTExpression* original, C3TypePtr type) : ASTExpression(type, original->is_constant), original(original) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTCast();
};

struct ASTCondition : ASTNode {
	ASTExpression* condition;
	ASTNode* true_path;
	ASTNode* false_path;

	ASTCondition(ASTExpression* condition, ASTNode* true_path, ASTNode* false_path) : condition(condition), true_path(true_path), false_path(false_path) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTCondition();
};

struct ASTWhileLoop : ASTNode {
	ASTExpression* condition;
	ASTNode* body;

	ASTWhileLoop(ASTExpression* condition, ASTNode* body) : condition(condition), body(body) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTWhileLoop();
};

struct ASTNullPointer : ASTExpression {
	ASTNullPointer(C3TypePtr type) : ASTExpression(type, true) {}
	virtual void print(int indentation = 0);
	virtual const void* accept(ASTNodeVisitor* visitor);
	virtual ~ASTNullPointer() = default;
};

class ASTNodeVisitor {
	public:
		virtual const void* visit(ASTNode* node) { return nullptr; }
		virtual const void* visit(ASTNop* node) { return nullptr; }
		virtual const void* visit(ASTExpression* node) { return nullptr; }
		virtual const void* visit(ASTSequence* node) { return nullptr; }
		virtual const void* visit(ASTVariableRef* node) { return nullptr; }
		virtual const void* visit(ASTVariableDec* node) { return nullptr; }
		virtual const void* visit(ASTFunctionRef* node) { return nullptr; }
		virtual const void* visit(ASTFunctionProto* node) { return nullptr; }
		virtual const void* visit(ASTFunctionDef* node) { return nullptr; }
		virtual const void* visit(ASTStructMemberRef* node) { return nullptr; }
		virtual const void* visit(ASTFloatingPoint* node) { return nullptr; }
		virtual const void* visit(ASTInteger* node) { return nullptr; }
		virtual const void* visit(ASTConstantArray* node) { return nullptr; }
		virtual const void* visit(ASTUnaryOp* node) { return nullptr; }
		virtual const void* visit(ASTBinaryOp* node) { return nullptr; }
		virtual const void* visit(ASTReturn* node) { return nullptr; }
		virtual const void* visit(ASTInlineAsm* node) { return nullptr; }
		virtual const void* visit(ASTFunctionCall* node) { return nullptr; }
		virtual const void* visit(ASTCast* node) { return nullptr; }
		virtual const void* visit(ASTCondition* node) { return nullptr; }
		virtual const void* visit(ASTWhileLoop* node) { return nullptr; }
		virtual const void* visit(ASTNullPointer* node) { return nullptr; }
		virtual ~ASTNodeVisitor() {}
};
