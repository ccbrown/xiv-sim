#pragma once

#include "C3Type.h"
#include "C3FunctionSignature.h"
#include "../Token.h"

#include <string>
#include <memory>
#include <vector>

class C3Function;
typedef std::shared_ptr<C3Function> C3FunctionPtr;

class C3Function {
	public:
		C3Function(C3TypePtr return_type, const std::string& name, const std::string& global_name, const std::vector<C3TypePtr>&& arg_types, const TokenPtr& prototype) : 
			_name(name), _global_name(global_name), _signature(return_type, std::move(arg_types)), _prototype(prototype), _definition(nullptr) {
			_type = C3Type::FunctionType(_signature);
		}

		C3TypePtr return_type() { return _signature.return_type(); }
		const std::string& name() { return _name; }

		/**
		* The global name is the mangled function name used in the compiled binary.
		*/
		const std::string& global_name() { return _global_name; }
		void set_global_name(const std::string& name) { _global_name = name; }

		const std::vector<C3TypePtr>& arg_types() { return _signature.arg_types(); }
		const C3FunctionSignature& signature() { return _signature; }
		C3TypePtr type() { return _type; }

		TokenPtr& prototype() { return _prototype; }
		void set_definition(const TokenPtr& tok) { _definition = tok; }
		TokenPtr& definition() { return _definition; }

	private:
		C3TypePtr _type;
		std::string _name;
		std::string _global_name;
		C3FunctionSignature _signature;
		
		TokenPtr _prototype;
		TokenPtr _definition;
};
