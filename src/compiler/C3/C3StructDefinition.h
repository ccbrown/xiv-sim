#pragma once

#include "C3TypePtr.h"

#include <vector>
#include <string>

class C3StructDefinition {
	public:
		struct MemberVariable {
			MemberVariable(std::string name, C3TypePtr type) : name(name), type(type) {}

			std::string name;
			C3TypePtr type;
		};
	
		C3StructDefinition() {}
		C3StructDefinition(const std::vector<MemberVariable>&& member_vars) : _member_vars(member_vars) {}
		
		size_t size() const;
		
		const std::vector<MemberVariable>& member_vars() const { return _member_vars; }

	private:
		std::vector<MemberVariable> _member_vars;
};
