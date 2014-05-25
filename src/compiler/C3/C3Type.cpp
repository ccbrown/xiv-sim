#include "C3Type.h"

#include <assert.h>

C3Type::C3Type(const std::string& name, const std::string& global_name, C3TypeType type) : _name(name), _global_name(global_name), _type(type) {
}

C3Type::C3Type(const std::string& name, C3TypeType type) : _name(name), _global_name(name), _type(type) {
}

C3Type::C3Type(const std::string& name, C3TypeType type, C3TypePtr pointed_to_or_referenced_type) : _name(name), _global_name(name), _type(type), _pointed_to_or_referenced_type(pointed_to_or_referenced_type) {
	assert(type == C3TypeTypePointer || type == C3TypeTypeReference);
}

C3Type::C3Type(const C3FunctionSignature& signature) : _name(signature.string()), _global_name(_name), _type(C3TypeTypeFunction), _function_sig(signature) {
}

std::string C3Type::name() const {
	std::string ret = "";
	if (is_constant()) {
		ret += "const ";
	}
	if (!is_signed()) {
		ret += "unsigned ";
	}
	return ret + _name;
}

size_t C3Type::size() const {
	switch (_type) {
		case C3TypeTypePointer:
		case C3TypeTypeReference:
		case C3TypeTypeNullPointer:
		case C3TypeTypeFunction:
			return sizeof(void*);
		case C3TypeTypeStruct:
			return _is_defined ? _struct_def.size() : 0;
		case C3TypeTypeAuto:
		case C3TypeTypeVoid:
			return 0;
		case C3TypeTypeBool:
		case C3TypeTypeInt8:
			return 1;
		case C3TypeTypeInt32:
			return 8;
		case C3TypeTypeInt64:
			return 8;
		case C3TypeTypeDouble:
			return sizeof(double);
	}
	
	assert(false);
	return 0;
}

C3TypePtr C3Type::pointed_to_type() const {
	return type() == C3TypeTypePointer ? _pointed_to_or_referenced_type : nullptr;
}

C3TypePtr C3Type::referenced_type() const {
	return type() == C3TypeTypeReference ? _pointed_to_or_referenced_type : nullptr;
}

const C3FunctionSignature& C3Type::signature() const {
	return _function_sig;
}

bool C3Type::is_integer() const {
	return (_type == C3TypeTypeInt8 || _type == C3TypeTypeInt32 || _type == C3TypeTypeInt64);
}

bool C3Type::is_floating_point() const {
	return (_type == C3TypeTypeDouble);
}

bool C3Type::is_auto() const {
	return (_type == C3TypeTypeAuto || (_pointed_to_or_referenced_type && _pointed_to_or_referenced_type->is_auto()));
}

bool C3Type::is_constant() const {
	return (_modifiers & C3TypeModifierConstant);
}

bool C3Type::is_signed() const {
	return !(_modifiers & C3TypeModifierUnsigned);
}

bool C3Type::is_defined() const {
	return (_type != C3TypeTypeStruct || _is_defined);
}

void C3Type::set_modifiers(int modifiers) {
	_modifiers = modifiers;
}

void C3Type::define(const C3StructDefinition& definition) {
	_struct_def = definition;
	_is_defined = true;
}

bool C3Type::operator==(const C3Type& right) const {
	return (true 
		&& type() == right.type() 
		&& is_constant() == right.is_constant() 
		&& is_signed() == right.is_signed() 
		&& (type() != C3TypeTypePointer || *(pointed_to_type()) == *(right.pointed_to_type()))
	);
}

bool C3Type::operator!=(const C3Type& right) const {
	return !(*this == right);
}

C3TypePtr C3Type::PointerType(C3TypePtr type) {
	if (!type->_pointer) {
		type->_pointer = C3TypePtr(new C3Type(type->name() + "*", C3TypeTypePointer, type));
	}
	return type->_pointer;
}

C3TypePtr C3Type::ReferenceType(C3TypePtr type) {
	if (!type->_reference) {
		type->_reference = C3TypePtr(new C3Type(type->name() + "&", C3TypeTypeReference, type));
	}
	return type->_reference;
}

C3TypePtr C3Type::AutoType() {
	static C3TypePtr ret = C3TypePtr(new C3Type("auto", C3TypeTypeAuto));
	return ret;
}

C3TypePtr C3Type::VoidType() {
	static C3TypePtr ret = C3TypePtr(new C3Type("void", C3TypeTypeVoid));
	return ret;
}

C3TypePtr C3Type::NullPointerType() {
	static C3TypePtr ret = C3TypePtr(new C3Type("nullptr", C3TypeTypeNullPointer));
	return ret;
}

C3TypePtr C3Type::BoolType() {
	static C3TypePtr ret = C3TypePtr(new C3Type("bool", C3TypeTypeBool));
	return ret;
}

C3TypePtr C3Type::FunctionType(const C3FunctionSignature& signature) {
	return C3TypePtr(new C3Type(signature));
}

C3TypePtr C3Type::StructType(const std::string& name, const std::string& global_name, const C3StructDefinition& definition) {
	auto type = C3TypePtr(new C3Type(name, global_name, C3TypeTypeStruct));
	type->define(definition);
	return type;
}

C3TypePtr C3Type::ModifiedType(C3TypePtr type, int modifiers) {
	auto ret = C3TypePtr(new C3Type(*type));
	ret->set_modifiers(modifiers);
	return ret;
}

C3TypePtr C3Type::Int8Type() {
	static C3TypePtr ret = C3TypePtr(new C3Type("int8", C3TypeTypeInt8));
	return ret;
}

C3TypePtr C3Type::Int32Type() {
	static C3TypePtr ret = C3TypePtr(new C3Type("int32", C3TypeTypeInt32));
	return ret;
}

C3TypePtr C3Type::Int64Type() {
	static C3TypePtr ret = C3TypePtr(new C3Type("int64", C3TypeTypeInt64));
	return ret;
}

C3TypePtr C3Type::DoubleType() {
	static C3TypePtr ret = C3TypePtr(new C3Type("double", C3TypeTypeDouble));
	return ret;
}

C3TypePtr C3Type::RemoveReference(C3TypePtr type) {
	return type->type() == C3TypeTypeReference ? type->referenced_type() : type;
}
