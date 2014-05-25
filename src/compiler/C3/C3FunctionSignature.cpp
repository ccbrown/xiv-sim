#include "C3FunctionSignature.h"

#include "C3Type.h"

C3FunctionSignature::C3FunctionSignature() {
}

C3FunctionSignature::C3FunctionSignature(C3TypePtr return_type, const std::vector<C3TypePtr>&& arg_types) : _return_type(return_type), _arg_types(arg_types) {
	_string = return_type->name() + '(';
	bool first = true;
	for (C3TypePtr type : arg_types) {
		if (first) {
			first = false;
		} else {
			_string += ", ";
		}
		_string += type->name();
	}
	_string += ')';
}

C3TypePtr C3FunctionSignature::return_type() const {
	return _return_type;
}

const std::vector<C3TypePtr>& C3FunctionSignature::arg_types() const {
	return _arg_types;
}

const std::string& C3FunctionSignature::string() const {
	return _string;
}

bool C3FunctionSignature::operator==(C3FunctionSignature other) const {
	if (*return_type() != *other.return_type() || arg_types().size() != other.arg_types().size()) {
		return false;
	}
	for (size_t i = 0; i < arg_types().size(); ++i) {
		if (*(arg_types()[i]) != *(other.arg_types()[i])) {
			return false;
		}
	}
	return true;
}

bool C3FunctionSignature::operator!=(C3FunctionSignature other) const {
	return !(*this == other);
}
