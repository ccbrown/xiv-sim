#pragma once

#include "C3Type.h"
#include "../Token.h"

#include <string>
#include <memory>

class C3Variable {
	public:
		C3Variable(C3TypePtr type, const std::string& name, const std::string& global_name, TokenPtr declaration, bool is_static = false)
			: _type(type), _name(name), _global_name(global_name), _declaration(declaration), _is_static(is_static)
		{}

		C3TypePtr type() { return _type; }
		const std::string& name() { return _name; }
		const std::string& global_name() { return _global_name; }
		TokenPtr declaration() { return _declaration; }
			
		bool is_static() { return _is_static; }

	private:
		C3TypePtr _type;
		std::string _name;
		std::string _global_name;
		TokenPtr _declaration;

		bool _is_static = false;
};

typedef std::shared_ptr<C3Variable> C3VariablePtr;
