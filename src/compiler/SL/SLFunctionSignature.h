#pragma once

#include "SLTypePtr.h"

#include <vector>
#include <string>

class SLFunctionSignature {
	public:
		SLFunctionSignature();
		SLFunctionSignature(SLTypePtr return_type, const std::vector<SLTypePtr>&& arg_types);

		SLTypePtr return_type() const;
		const std::vector<SLTypePtr>& arg_types() const;
		
		const std::string& string() const;

		bool operator==(SLFunctionSignature other) const;
		bool operator!=(SLFunctionSignature other) const;
		
	private:
		SLTypePtr _return_type;
		std::vector<SLTypePtr> _arg_types;

		std::string _string;
};
