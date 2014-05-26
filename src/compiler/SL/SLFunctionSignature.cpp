#include "SLFunctionSignature.h"

#include "SLType.h"

SLFunctionSignature::SLFunctionSignature() {
}

SLFunctionSignature::SLFunctionSignature(SLTypePtr return_type, const std::vector<SLTypePtr>&& arg_types) : _return_type(return_type), _arg_types(arg_types) {
	_string = return_type->name() + '(';
	bool first = true;
	for (SLTypePtr type : arg_types) {
		if (first) {
			first = false;
		} else {
			_string += ", ";
		}
		_string += type->name();
	}
	_string += ')';
}

SLTypePtr SLFunctionSignature::return_type() const {
	return _return_type;
}

const std::vector<SLTypePtr>& SLFunctionSignature::arg_types() const {
	return _arg_types;
}

const std::string& SLFunctionSignature::string() const {
	return _string;
}

bool SLFunctionSignature::operator==(SLFunctionSignature other) const {
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

bool SLFunctionSignature::operator!=(SLFunctionSignature other) const {
	return !(*this == other);
}
