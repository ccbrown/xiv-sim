#include "SLType.h"

#include <assert.h>

SLType::SLType(const std::string& name, const std::string& global_name, SLTypeType type) : _name(name), _global_name(global_name), _type(type) {
}

SLType::SLType(const std::string& name, SLTypeType type) : _name(name), _global_name(name), _type(type) {
}

SLType::SLType(const std::string& name, SLTypeType type, SLTypePtr pointed_to_or_referenced_type) : _name(name), _global_name(name), _type(type), _pointed_to_or_referenced_type(pointed_to_or_referenced_type) {
	assert(type == SLTypeTypePointer || type == SLTypeTypeReference);
}

SLType::SLType(const SLFunctionSignature& signature) : _name(signature.string()), _global_name(_name), _type(SLTypeTypeFunction), _function_sig(signature) {
}

std::string SLType::name() const {
	std::string ret = "";
	if (is_constant()) {
		ret += "const ";
	}
	if (!is_signed()) {
		ret += "unsigned ";
	}
	return ret + _name;
}

size_t SLType::size() const {
	switch (_type) {
		case SLTypeTypePointer:
		case SLTypeTypeReference:
		case SLTypeTypeNullPointer:
		case SLTypeTypeFunction:
			return sizeof(void*);
		case SLTypeTypeStruct:
			return _is_defined ? _struct_def.size() : 0;
		case SLTypeTypeAuto:
		case SLTypeTypeVoid:
			return 0;
		case SLTypeTypeBool:
		case SLTypeTypeInt8:
			return 1;
		case SLTypeTypeInt32:
			return 8;
		case SLTypeTypeInt64:
			return 8;
		case SLTypeTypeDouble:
			return sizeof(double);
	}
	
	assert(false);
	return 0;
}

SLTypePtr SLType::pointed_to_type() const {
	return type() == SLTypeTypePointer ? _pointed_to_or_referenced_type : nullptr;
}

SLTypePtr SLType::referenced_type() const {
	return type() == SLTypeTypeReference ? _pointed_to_or_referenced_type : nullptr;
}

const SLFunctionSignature& SLType::signature() const {
	return _function_sig;
}

bool SLType::is_integer() const {
	return (_type == SLTypeTypeInt8 || _type == SLTypeTypeInt32 || _type == SLTypeTypeInt64);
}

bool SLType::is_floating_point() const {
	return (_type == SLTypeTypeDouble);
}

bool SLType::is_auto() const {
	return (_type == SLTypeTypeAuto || (_pointed_to_or_referenced_type && _pointed_to_or_referenced_type->is_auto()));
}

bool SLType::is_constant() const {
	return (_modifiers & SLTypeModifierConstant);
}

bool SLType::is_signed() const {
	return !(_modifiers & SLTypeModifierUnsigned);
}

bool SLType::is_defined() const {
	return (_type != SLTypeTypeStruct || _is_defined);
}

void SLType::set_modifiers(int modifiers) {
	_modifiers = modifiers;
}

void SLType::define(const SLStructDefinition& definition) {
	_struct_def = definition;
	_is_defined = true;
}

bool SLType::operator==(const SLType& right) const {
	return (true 
		&& type() == right.type() 
		&& is_constant() == right.is_constant() 
		&& is_signed() == right.is_signed() 
		&& (type() != SLTypeTypePointer || *(pointed_to_type()) == *(right.pointed_to_type()))
	);
}

bool SLType::operator!=(const SLType& right) const {
	return !(*this == right);
}

SLTypePtr SLType::PointerType(SLTypePtr type) {
	if (!type->_pointer) {
		type->_pointer = SLTypePtr(new SLType(type->name() + "*", SLTypeTypePointer, type));
	}
	return type->_pointer;
}

SLTypePtr SLType::ReferenceType(SLTypePtr type) {
	if (!type->_reference) {
		type->_reference = SLTypePtr(new SLType(type->name() + "&", SLTypeTypeReference, type));
	}
	return type->_reference;
}

SLTypePtr SLType::AutoType() {
	static SLTypePtr ret = SLTypePtr(new SLType("auto", SLTypeTypeAuto));
	return ret;
}

SLTypePtr SLType::VoidType() {
	static SLTypePtr ret = SLTypePtr(new SLType("void", SLTypeTypeVoid));
	return ret;
}

SLTypePtr SLType::NullPointerType() {
	static SLTypePtr ret = SLTypePtr(new SLType("nullptr", SLTypeTypeNullPointer));
	return ret;
}

SLTypePtr SLType::BoolType() {
	static SLTypePtr ret = SLTypePtr(new SLType("bool", SLTypeTypeBool));
	return ret;
}

SLTypePtr SLType::FunctionType(const SLFunctionSignature& signature) {
	return SLTypePtr(new SLType(signature));
}

SLTypePtr SLType::StructType(const std::string& name, const std::string& global_name) {
	return SLTypePtr(new SLType(name, global_name, SLTypeTypeStruct));
}

SLTypePtr SLType::ModifiedType(SLTypePtr type, int modifiers) {
	auto ret = SLTypePtr(new SLType(*type));
	ret->set_modifiers(modifiers);
	return ret;
}

SLTypePtr SLType::Int8Type() {
	static SLTypePtr ret = SLTypePtr(new SLType("int8", SLTypeTypeInt8));
	return ret;
}

SLTypePtr SLType::Int32Type() {
	static SLTypePtr ret = SLTypePtr(new SLType("int32", SLTypeTypeInt32));
	return ret;
}

SLTypePtr SLType::Int64Type() {
	static SLTypePtr ret = SLTypePtr(new SLType("int64", SLTypeTypeInt64));
	return ret;
}

SLTypePtr SLType::DoubleType() {
	static SLTypePtr ret = SLTypePtr(new SLType("double", SLTypeTypeDouble));
	return ret;
}

SLTypePtr SLType::RemoveReference(SLTypePtr type) {
	return type->type() == SLTypeTypeReference ? type->referenced_type() : type;
}
