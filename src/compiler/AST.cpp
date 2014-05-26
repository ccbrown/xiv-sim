#include "AST.h"

void ASTNode::print(int indentation) {
	printf("%*snode\n", indentation * 2, "");
}

const void* ASTNode::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTNop::print(int indentation) {
	printf("%*snop\n", indentation * 2, "");
}

const void* ASTNop::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTExpression::print(int indentation) {
	printf("%*sexpression\n", indentation * 2, "");
}

const void* ASTExpression::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTSequence::print(int indentation) {
	printf("%*ssequence\n", indentation * 2, "");
	for (ASTNode* node : sequence) {
		node->print(indentation + 1);
	}
}

const void* ASTSequence::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTSequence::~ASTSequence() {
	for (ASTNode* n : sequence) {
		delete n;
	}
}

void ASTVariableRef::print(int indentation) {
	printf("%*sref: %s (%s)\n", indentation * 2, "", var->name().c_str(), var->global_name().c_str());
}

const void* ASTVariableRef::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTVariableDec::print(int indentation) {
	printf("%*svariable dec: %s %s (%s)\n", indentation * 2, "", var->type()->name().c_str(), var->name().c_str(), var->global_name().c_str());
	if (init) {
		init->print(indentation + 1);
	}
}

const void* ASTVariableDec::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTFunctionRef::print(int indentation) {
	printf("%*sfunction ref: %s (%s)\n", indentation * 2, "", func->name().c_str(), func->global_name().c_str());
}

const void* ASTFunctionRef::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTFunctionProto::print(int indentation) {
	printf("%*sfunction proto: %s %s(", indentation * 2, "", func->return_type()->name().c_str(), func->name().c_str());
	const std::vector<SLTypePtr>& types = func->arg_types();
	for (size_t i = 0; i < types.size(); ++i) {
		printf(i == 0 ? "%s %s" : ", %s %s", types[i]->name().c_str(), arg_names[i].c_str());
	}
	printf(")\n");
}

const void* ASTFunctionProto::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTFunctionDef::print(int indentation) {
	proto->print(indentation);
	printf("%*sfunction body\n", indentation * 2, "");
	body->print(indentation + 1);
}

const void* ASTFunctionDef::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTFunctionDef::~ASTFunctionDef() {
	delete body;
}

ASTStructMemberRef::ASTStructMemberRef(ASTExpression* structure, size_t index) 
	: ASTExpression(SLType::ReferenceType(SLType::RemoveReference(structure->type)->struct_definition().member_vars()[index].type))
	, structure(structure)
	, index(index)
{	
}

void ASTStructMemberRef::print(int indentation) {
	printf("%*smember ref, index = %zu\n", indentation * 2, "", index);
	structure->print(indentation + 1);
}

const void* ASTStructMemberRef::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTStructMemberRef::~ASTStructMemberRef() {
	delete structure;
}

void ASTFloatingPoint::print(int indentation) {
	printf("%*sfloating point: %f\n", indentation * 2, "", value);
}

const void* ASTFloatingPoint::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTInteger::print(int indentation) {
	printf("%*sinteger: %lu\n", indentation * 2, "", value);
}

const void* ASTInteger::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTConstantArray::print(int indentation) {
	printf("%*sarray: %s[%lu]\n", indentation * 2, "", type->pointed_to_type()->name().c_str(), size / type->pointed_to_type()->size());
}

const void* ASTConstantArray::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTUnaryOp::print(int indentation) {
	printf("%*sunary op: %s\n", indentation * 2, "", op.c_str());
	right->print(indentation + 1);
}

const void* ASTUnaryOp::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTUnaryOp::~ASTUnaryOp() {
	delete right;
}

void ASTBinaryOp::print(int indentation) {
	printf("%*sbinary op: %s\n", indentation * 2, "", op.c_str());
	left->print(indentation + 1);
	right->print(indentation + 1);
}

const void* ASTBinaryOp::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTBinaryOp::~ASTBinaryOp() {
	delete left;
	delete right;
}

void ASTReturn::print(int indentation) {
	printf("%*sreturn\n", indentation * 2, "");
	if (value) {
		value->print(indentation + 1);
	}
}

const void* ASTReturn::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

void ASTInlineAsm::print(int indentation) {
	printf("%*sinline asm\n", indentation * 2, "");
	printf("%*soutputs\n", (indentation + 1) * 2, "");
	for (ASTExpression* exp : outputs) {
		exp->print(indentation + 2);
	}
	printf("%*sinputs\n", (indentation + 1) * 2, "");
	for (ASTExpression* exp : inputs) {
		exp->print(indentation + 2);
	}
}

const void* ASTInlineAsm::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTInlineAsm::~ASTInlineAsm() {
	for (ASTExpression* exp : outputs) {
		delete exp;
	}
	for (ASTExpression* exp : inputs) {
		delete exp;
	}
}

void ASTFunctionCall::print(int indentation) {
	printf("%*sfunction call\n", indentation * 2, "");
	func->print(indentation + 1);
	printf("%*sarguments\n", (indentation + 1) * 2, "");
	for (ASTExpression* exp : args) {
		exp->print(indentation + 2);
	}
}

const void* ASTFunctionCall::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTFunctionCall::~ASTFunctionCall() {
	delete func;
	for (ASTExpression* exp : args) {
		delete exp;
	}
}

void ASTCast::print(int indentation) {
	printf("%*scast: %s\n", indentation * 2, "", type->name().c_str());
	original->print(indentation + 1);
}

const void* ASTCast::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTCast::~ASTCast() {
	delete original;
}

void ASTCondition::print(int indentation) {
	printf("%*scondition:\n", indentation * 2, "");
	condition->print(indentation + 1);
	printf("%*strue:\n", (indentation + 1) * 2, "");
	true_path->print(indentation + 2);
	printf("%*sfalse:\n", (indentation + 1) * 2, "");
	false_path->print(indentation + 2);
}

const void* ASTCondition::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTCondition::~ASTCondition() {
	delete condition;
	delete true_path;
	delete false_path;
}

void ASTWhileLoop::print(int indentation) {
	printf("%*swhile:\n", indentation * 2, "");
	condition->print(indentation + 1);
	printf("%*sloop:\n", (indentation + 1) * 2, "");
	body->print(indentation + 2);
}

const void* ASTWhileLoop::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}

ASTWhileLoop::~ASTWhileLoop() {
	delete condition;
	delete body;
}

void ASTNullPointer::print(int indentation) {
	printf("%*snull pointer\n", indentation * 2, "");
}

const void* ASTNullPointer::accept(ASTNodeVisitor* visitor) {
	return visitor->visit(this);
}