#pragma once

#include "SLType.h"
#include "../Token.h"

#include <string>
#include <memory>

class SLVariable {
	public:
		SLVariable(SLTypePtr type, const std::string& name, const std::string& global_name, TokenPtr declaration, bool is_static = false)
			: _type(type), _name(name), _global_name(global_name), _declaration(declaration), _is_static(is_static)
		{}

		SLTypePtr type() { return _type; }
		const std::string& name() { return _name; }
		const std::string& global_name() { return _global_name; }
		TokenPtr declaration() { return _declaration; }
			
		bool is_static() { return _is_static; }

	private:
		SLTypePtr _type;
		std::string _name;
		std::string _global_name;
		TokenPtr _declaration;

		bool _is_static = false;
};

typedef std::shared_ptr<SLVariable> SLVariablePtr;
