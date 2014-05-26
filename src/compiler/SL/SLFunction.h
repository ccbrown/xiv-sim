#pragma once

#include "SLType.h"
#include "SLFunctionSignature.h"
#include "../Token.h"

#include <string>
#include <memory>
#include <vector>

class SLFunction;
typedef std::shared_ptr<SLFunction> SLFunctionPtr;

class SLFunction {
	public:
		SLFunction(SLTypePtr return_type, const std::string& name, const std::string& global_name, const std::vector<SLTypePtr>&& arg_types, const TokenPtr& prototype) : 
			_name(name), _global_name(global_name), _signature(return_type, std::move(arg_types)), _prototype(prototype), _definition(nullptr) {
			_type = SLType::FunctionType(_signature);
		}

		SLTypePtr return_type() { return _signature.return_type(); }
		const std::string& name() { return _name; }

		/**
		* The global name is the mangled function name used in the compiled binary.
		*/
		const std::string& global_name() { return _global_name; }
		void set_global_name(const std::string& name) { _global_name = name; }

		const std::vector<SLTypePtr>& arg_types() { return _signature.arg_types(); }
		const SLFunctionSignature& signature() { return _signature; }
		SLTypePtr type() { return _type; }

		TokenPtr& prototype() { return _prototype; }
		void set_definition(const TokenPtr& tok) { _definition = tok; }
		TokenPtr& definition() { return _definition; }

	private:
		SLTypePtr _type;
		std::string _name;
		std::string _global_name;
		SLFunctionSignature _signature;
		
		TokenPtr _prototype;
		TokenPtr _definition;
};
