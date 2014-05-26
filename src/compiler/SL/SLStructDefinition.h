#pragma once

#include "SLTypePtr.h"

#include <vector>
#include <string>

class SLStructDefinition {
	public:
		struct MemberVariable {
			MemberVariable(std::string name, SLTypePtr type) : name(name), type(type) {}

			std::string name;
			SLTypePtr type;
		};
	
		SLStructDefinition() {}
		SLStructDefinition(const std::vector<MemberVariable>&& member_vars) : _member_vars(member_vars) {}
		
		size_t size() const;
		
		const std::vector<MemberVariable>& member_vars() const { return _member_vars; }

	private:
		std::vector<MemberVariable> _member_vars;
};
