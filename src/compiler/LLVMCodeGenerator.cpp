#include "LLVMCodeGenerator.h"

#include <llvm/IR/InlineAsm.h>
#include <llvm/PassManager.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Assembly/PrintModulePass.h>

LLVMCodeGenerator::LLVMCodeGenerator() 
	: _context(llvm::getGlobalContext())
	, _module(new llvm::Module("top", _context))
	, _builder(llvm::getGlobalContext())
{
}

LLVMCodeGenerator::~LLVMCodeGenerator() {
	delete _module;
}

bool LLVMCodeGenerator::build_ir(ASTNode* ast) {
	ast->accept(this);

	return !llvm::verifyModule(*_module, llvm::PrintMessageAction); // verifyModule returns false on success
}

llvm::Module* LLVMCodeGenerator::release_module() {
	auto ret = _module;
	_module = nullptr;
	return ret;
}

const void* LLVMCodeGenerator::visit(ASTNode* node) {
	assert(false);
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTNop* node) {
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTExpression* node) {
	assert(false);
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTSequence* node) {
	for (ASTNode* n : node->sequence) {
		n->accept(this);
	}
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTVariableRef* node) {
	llvm::Value* v = _named_values[node->var->global_name()];
	assert(v);
	return v;
}

const void* LLVMCodeGenerator::visit(ASTVariableDec* node) {
	if (node->var->is_static()) {
		assert(node->init->is_constant);
		auto global = new llvm::GlobalVariable(*_module, _llvm_type(node->var->type()), node->var->type()->is_constant(), llvm::GlobalValue::WeakAnyLinkage, static_cast<llvm::Constant*>(_value(node->init)), node->var->global_name().c_str());
		_named_values[node->var->global_name()] = global;
	} else {
		auto alloca = _builder.CreateAlloca(_llvm_type(node->var->type()), 0, node->var->global_name().c_str());
		_named_values[node->var->global_name()] = alloca;
	
		if (node->init) {
			_builder.CreateStore(_dereferenced_value(node->init), alloca);
		}
	}

	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTFunctionRef* node) {
	llvm::Value* v = _module->getFunction(node->func->global_name());
	assert(v);
	return v;
}

const void* LLVMCodeGenerator::visit(ASTFunctionProto* node) {
	std::vector<llvm::Type*> arg_types;
	for (SLTypePtr type : node->func->arg_types()) {
		arg_types.push_back(_llvm_type(type));
	}

	const std::string& name = node->func->global_name();

	llvm::FunctionType* ft = llvm::FunctionType::get(_llvm_type(node->func->return_type()), arg_types, false);
	llvm::Function* f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, _module);

	if (f->getName() != name) {
		// function already exists. erase this, get the old one
		f->eraseFromParent();
		f = _module->getFunction(name);
	}

	return f;
}

const void* LLVMCodeGenerator::visit(ASTFunctionDef* node) {
	auto function = (llvm::Function*)node->proto->accept(this);
	
	// set names
	size_t i = 0;
	for (llvm::Function::arg_iterator ai = function->arg_begin(); ai != function->arg_end(); ++ai) {
		ai->setName(node->proto->arg_names[i]);
		++i;
	}

	// create the block
	llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(_context, "entry", function);
	llvm::IRBuilderBase::InsertPoint ip = _builder.saveIP();
	_builder.SetInsertPoint(entry_block);

	// load arguments
	// we copy them to allow them to be lvalues
	// TODO: make sure llvm can optimize the unnecessary copies out?
	i = 0;
	for (llvm::Function::arg_iterator ai = function->arg_begin(); ai != function->arg_end(); ++ai) {
		llvm::AllocaInst* alloca = _builder.CreateAlloca(ai->getType(), 0, node->arg_prefix + node->proto->arg_names[i]);
		_named_values[node->arg_prefix + node->proto->arg_names[i]] = alloca;
		_builder.CreateStore(ai, alloca);
		++i;
	}

	// build the body

	llvm::AllocaInst* return_alloca = nullptr;
	if (node->proto->func->return_type()->type() != SLTypeTypeVoid) {
		return_alloca = _builder.CreateAlloca(_llvm_type(node->proto->func->return_type()), nullptr, "ret");
	}

	llvm::BasicBlock* return_block = llvm::BasicBlock::Create(_context, "return", function);

	FunctionContext context(node->proto->func, function, return_alloca, return_block);
	auto prev_function_context = _current_function_context;
	_current_function_context = context;

	_build_basic_block(entry_block, node->body, return_block);

	_current_function_context = prev_function_context;

	_builder.SetInsertPoint(return_block);
	if (return_alloca) {
		_builder.CreateRet(_builder.CreateLoad(return_alloca));
	} else {
		_builder.CreateRetVoid();
	}

	// end the block
	_builder.restoreIP(ip);
	
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTStructMemberRef* node) {
	return _builder.CreateStructGEP((llvm::Value*)node->structure->accept(this), node->index);
}

const void* LLVMCodeGenerator::visit(ASTFloatingPoint* node) {
	return llvm::ConstantFP::get(_context, llvm::APFloat(node->value));
}

const void* LLVMCodeGenerator::visit(ASTInteger* node) {
	if (node->type->type() == SLTypeTypeBool) {
		return node->value ? llvm::ConstantInt::getTrue(_context) : llvm::ConstantInt::getFalse(_context);
	}
	return llvm::ConstantInt::get(_context, llvm::APInt(64, node->value));
}

const void* LLVMCodeGenerator::visit(ASTConstantArray* node) {
	llvm::Constant* v = nullptr;

	assert(node->type->pointed_to_type());
	switch (node->type->pointed_to_type()->type()) {
		case SLTypeTypeInt8:
			v = llvm::ConstantDataArray::get(_context, llvm::ArrayRef<uint8_t>((uint8_t*)node->data, node->size));
			break;
		default:
			break;
	}
	assert(v);

	llvm::GlobalVariable* gv = new llvm::GlobalVariable(*_module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v, "", 0, llvm::GlobalVariable::NotThreadLocal);
	gv->setUnnamedAddr(true);
	llvm::Value* zero   = llvm::ConstantInt::get(llvm::Type::getInt32Ty(_context), 0);
	llvm::Value* args[] = { zero, zero };
	return _builder.CreateInBoundsGEP(gv, args);
}

const void* LLVMCodeGenerator::visit(ASTUnaryOp* node) {
	if (node->op == "&") {
		assert(node->right->type->referenced_type());
		return _value(node->right);
	} else if (node->op == "*") {
		assert(SLType::RemoveReference(node->right->type)->pointed_to_type());
		return _dereferenced_value(node->right);
	} else if (node->op == "!") {
		return _builder.CreateNot(_dereferenced_value(node->right));
	} else if (node->op == "-") {
		auto value = _dereferenced_value(node->right);
		return value->getType()->isFPOrFPVectorTy() ? _builder.CreateFNeg(value) : _builder.CreateNeg(value);
	}

	assert(false);
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTBinaryOp* node) {
	if (node->op == "=") {
		// assign
		llvm::Value* left = _value(node->left);
		_builder.CreateStore(_dereferenced_value(node->right), left);
		return left;
	} else {
		auto left       = _dereferenced_value(node->left);
		auto right      = _dereferenced_value(node->right);
		bool signed_op  = node->left->type->is_signed() || node->right->type->is_signed();
		if (node->op == "*") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFMul(left, right) : _builder.CreateMul(left, right);
		} else if (node->op == "/") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFDiv(left, right) : (signed_op ? _builder.CreateSDiv(left, right) : _builder.CreateUDiv(left, right));
		} else if (node->op == "%") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFRem(left, right) : (signed_op ? _builder.CreateSRem(left, right) : _builder.CreateURem(left, right));
		} else if (node->op == "+") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFAdd(left, right) : _builder.CreateAdd(left, right);
		} else if (node->op == "-") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFSub(left, right) : _builder.CreateSub(left, right);
		} else if (node->op == "==") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFCmpOEQ(left, right) : _builder.CreateICmpEQ(left, right);
		} else if (node->op == "!=") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFCmpONE(left, right) : _builder.CreateICmpNE(left, right);
		} else if (node->op == "<") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFCmpOLT(left, right) : (signed_op ? _builder.CreateICmpSLT(left, right) : _builder.CreateICmpULT(left, right));
		} else if (node->op == "<=") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFCmpOLE(left, right) : (signed_op ? _builder.CreateICmpSLE(left, right) : _builder.CreateICmpULE(left, right));
		} else if (node->op == ">") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFCmpOGT(left, right) : (signed_op ? _builder.CreateICmpSGT(left, right) : _builder.CreateICmpUGT(left, right));
		} else if (node->op == ">=") {
			return left->getType()->isFPOrFPVectorTy() ? _builder.CreateFCmpOGE(left, right) : (signed_op ? _builder.CreateICmpSGE(left, right) : _builder.CreateICmpUGE(left, right));
		} else if (node->op == "&&" || node->op == "and") {
			return _builder.CreateAnd(left, right);
		} else if (node->op == "||" || node->op == "or") {
			return _builder.CreateOr(left, right);
		}
	}

	assert(false);
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTReturn* node) {
	assert(_current_function_context.c3_function);
	
	if (node->value) {
		_builder.CreateStore(_dereferenced_value(node->value), _current_function_context.return_alloca);
	}

	_builder.CreateBr(_current_function_context.return_block);
	_is_current_block_terminated = true;

	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTInlineAsm* node) {
	std::vector<llvm::Type*> input_types;
	std::vector<llvm::Value*> args;

	size_t i = 0;
	for (ASTExpression* exp : node->inputs) {
		if (node->constraints[node->outputs.size() + i].find('*') != std::string::npos) {
			// indirect
			args.push_back(_value(exp));
			input_types.push_back(_llvm_type(exp->type)->getPointerTo());
		} else {
			args.push_back(_dereferenced_value(exp));
			input_types.push_back(_llvm_type(exp->type));
		}
		++i;
	}

	std::vector<llvm::Type*> output_types;
	for (ASTExpression* exp : node->outputs) {
		output_types.push_back(_llvm_type(exp->type));
	}

	llvm::Type* type = _builder.getVoidTy();
	
	if (output_types.size() == 1) {
		type = output_types[0];
	} else if (output_types.size() > 1) {
		type = llvm::StructType::create(output_types);
	}

	std::string constraint_str = "";
	bool first = true;
	for (std::string& c : node->constraints) {
		if (first) {
			first = false;
		} else {
			constraint_str += ',';
		}
		constraint_str += c;
	}

	llvm::InlineAsm* as = llvm::InlineAsm::get(llvm::FunctionType::get(type, input_types, false), node->assembly, constraint_str, true);
	_builder.Insert(llvm::CallInst::Create(as, args));
	
	// TODO: store outputs
	
	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTFunctionCall* node) {
	std::vector<llvm::Value*> args;
	for (ASTExpression* exp : node->args) {
		args.push_back(_dereferenced_value(exp));
	}
	return _builder.CreateCall(_dereferenced_value(node->func), args);
}

const void* LLVMCodeGenerator::visit(ASTCast* node) {
	if (node->original->is_constant) {
		// constant expression
		if (node->type->type() == SLTypeTypePointer) {
			return llvm::ConstantExpr::getPointerCast(static_cast<llvm::Constant*>(_value(node->original)), _llvm_type(node->type));
		}
		return llvm::ConstantExpr::getIntegerCast(static_cast<llvm::Constant*>(_value(node->original)), _llvm_type(node->type), node->original->type->is_signed());
	}
	
	auto rr_type = SLType::RemoveReference(node->original->type);
	
	if (node->type->type() == SLTypeTypePointer) {
		return _builder.CreatePointerCast(_dereferenced_value(node->original), _llvm_type(node->type));
	}

	if (node->type->type() == SLTypeTypeBool && rr_type->type() == SLTypeTypePointer) {
		return _builder.CreateIsNotNull(_dereferenced_value(node->original));
	}

	if (node->type->type() == SLTypeTypeBool && rr_type->is_integer()) {
		return _builder.CreateICmpNE(_dereferenced_value(node->original), llvm::ConstantInt::get(_llvm_type(rr_type), 0));
	}

	if (node->type->type() == SLTypeTypeBool && rr_type->is_floating_point()) {
		return _builder.CreateFCmpONE(_dereferenced_value(node->original), llvm::ConstantFP::get(_llvm_type(rr_type), 0.0));
	}

	return _builder.CreateIntCast(_dereferenced_value(node->original), _llvm_type(node->type), node->original->type->is_signed());
}

const void* LLVMCodeGenerator::visit(ASTCondition* node) {
	assert(_current_function_context.llvm_function);
	
	llvm::BasicBlock* post_block = llvm::BasicBlock::Create(_context, "post", _current_function_context.llvm_function);

	// create the true block
	llvm::BasicBlock* true_block = llvm::BasicBlock::Create(_context, "true", _current_function_context.llvm_function);
	_build_basic_block(true_block, node->true_path, post_block);

	// create the false block
	llvm::BasicBlock* false_block = llvm::BasicBlock::Create(_context, "false", _current_function_context.llvm_function);
	_build_basic_block(false_block, node->false_path, post_block);

	_builder.CreateCondBr(_dereferenced_value(node->condition), true_block, false_block);
	_builder.SetInsertPoint(post_block);
	_is_current_block_terminated = false;

	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTWhileLoop* node) {
	assert(_current_function_context.llvm_function);

	llvm::BasicBlock* while_block = llvm::BasicBlock::Create(_context, "while", _current_function_context.llvm_function);
	llvm::BasicBlock* break_block = llvm::BasicBlock::Create(_context, "break", _current_function_context.llvm_function);

	_builder.CreateBr(while_block);

	// create the loop block
	llvm::BasicBlock* body_block = llvm::BasicBlock::Create(_context, "body", _current_function_context.llvm_function);
	_build_basic_block(body_block, node->body, while_block);

	_builder.SetInsertPoint(while_block);
	_builder.CreateCondBr(_dereferenced_value(node->condition), body_block, break_block);
	_builder.SetInsertPoint(break_block);
	_is_current_block_terminated = false;

	return nullptr;
}

const void* LLVMCodeGenerator::visit(ASTNullPointer* node) {
	return llvm::ConstantPointerNull::get(static_cast<llvm::PointerType*>(_llvm_type(node->type)));
}

llvm::Value* LLVMCodeGenerator::_value(ASTExpression* exp) {
	return (llvm::Value*)exp->accept(this);
}

llvm::Value* LLVMCodeGenerator::_dereferenced_value(ASTExpression* exp) {
	llvm::Value* v = (llvm::Value*)exp->accept(this);
	if (exp->type->referenced_type()) {
		v = _builder.CreateLoad(v);
	}
	return v;
}

llvm::Type* LLVMCodeGenerator::_llvm_type(SLTypePtr type) {
	switch (type->type()) {
		case SLTypeTypeNullPointer:
			return llvm::Type::getInt8Ty(_context)->getPointerTo();
		case SLTypeTypePointer:
		case SLTypeTypeReference:
			// llvm doesn't do void pointers
			return type->pointed_to_type()->type() == SLTypeTypeVoid ? llvm::Type::getInt8Ty(_context)->getPointerTo() : _llvm_type(type->pointed_to_type())->getPointerTo();
		case SLTypeTypeAuto:
			assert(false);
		case SLTypeTypeVoid:
			return llvm::Type::getVoidTy(_context);
		case SLTypeTypeBool:
			return llvm::Type::getInt1Ty(_context);
		case SLTypeTypeInt8:
			return llvm::Type::getInt8Ty(_context);
		case SLTypeTypeInt32:
			return llvm::Type::getInt32Ty(_context);
		case SLTypeTypeInt64:
			return llvm::Type::getInt64Ty(_context);
		case SLTypeTypeDouble:
			return llvm::Type::getDoubleTy(_context);
		case SLTypeTypeFunction:
			assert(false); // TODO: ???
		case SLTypeTypeStruct: {
			llvm::StructType* ret = nullptr;
			
			if (!_named_types.count(type->global_name())) {
				_named_types[type->global_name()] = ret = llvm::StructType::create(_context, type->global_name());
			} else {
				auto t = _named_types[type->global_name()];
				assert(t->isStructTy());
				ret = (llvm::StructType*)t;
			}
			
			if (ret->isOpaque() && type->is_defined()) {
				std::vector<llvm::Type*> elements;
				auto& member_vars = type->struct_definition().member_vars();
				for (auto& var : member_vars) {
					elements.push_back(_llvm_type(var.type));
				}
				ret->setBody(elements, true);
			}
			
			return ret;
		}
	}
	
	assert(false);
	return llvm::Type::getVoidTy(_context);
}

void LLVMCodeGenerator::_build_basic_block(llvm::BasicBlock* block, ASTNode* node, llvm::BasicBlock* next) {
	auto ip = _builder.saveIP();
	_builder.SetInsertPoint(block);
	auto was_block_terminated = _is_current_block_terminated;
	_is_current_block_terminated = false;

	node->accept(this);
	if (!_is_current_block_terminated) {
		_builder.CreateBr(next);
	}

	_builder.restoreIP(ip);
	_is_current_block_terminated = was_block_terminated;
}
