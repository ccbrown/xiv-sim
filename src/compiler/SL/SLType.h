#pragma once

#include <string>
#include <memory>

enum SLTypeType {
	SLTypeTypePointer,
	SLTypeTypeReference,
	SLTypeTypeFunction,
	SLTypeTypeStruct,
	SLTypeTypeAuto,
	SLTypeTypeVoid,
	SLTypeTypeNullPointer,
	SLTypeTypeBool,
	SLTypeTypeInt8,
	SLTypeTypeInt32,
	SLTypeTypeInt64,
	SLTypeTypeDouble,
};

enum SLTypeModifier {
	SLTypeModifierNone     = 0,
	SLTypeModifierUnsigned = (1 << 0),
	SLTypeModifierConstant = (1 << 1),
	SLTypeModifierMask     = (1 << 2) - 1
};

class SLType;

#include "SLTypePtr.h"
#include "SLFunctionSignature.h"
#include "SLStructDefinition.h"

class SLType {
	public:
		std::string name() const;
		const std::string& global_name() const { return _global_name; }
		SLTypeType type() const { return _type; }
		size_t size() const;

		SLTypePtr pointed_to_type() const;
		SLTypePtr referenced_type() const;

		const SLFunctionSignature& signature() const;

		bool is_integer() const;
		bool is_floating_point() const;
		bool is_auto() const;

		bool is_constant() const;
		bool is_signed() const;
		bool is_defined() const;
		
		int modifiers() const { return _modifiers; }
		void set_modifiers(int modifiers);

		void define(const SLStructDefinition& definition);
		const SLStructDefinition& struct_definition() const { return _struct_def; }

		bool operator==(const SLType& right) const;
		bool operator!=(const SLType& right) const;

		static SLTypePtr PointerType(SLTypePtr type);
		static SLTypePtr ReferenceType(SLTypePtr type);
		static SLTypePtr FunctionType(const SLFunctionSignature& signature);
		static SLTypePtr StructType(const std::string& name, const std::string& global_name);
		static SLTypePtr ModifiedType(SLTypePtr type, int modifiers);
		static SLTypePtr AutoType();
		static SLTypePtr VoidType();
		static SLTypePtr NullPointerType();
		static SLTypePtr BoolType();
		static SLTypePtr Int8Type();
		static SLTypePtr Int32Type();
		static SLTypePtr Int64Type();
		static SLTypePtr DoubleType();

		static SLTypePtr RemoveReference(SLTypePtr type);

	private:
		SLType(const std::string& name, const std::string& global_name, SLTypeType type);
		SLType(const std::string& name, SLTypeType type);
		SLType(const std::string& name, SLTypeType type, SLTypePtr pointed_to_or_referenced);
		SLType(const SLFunctionSignature& signature);

		std::string _name;
		std::string _global_name;
		SLTypeType _type;
		
		int _modifiers = 0;

		bool _is_defined = false;

		SLTypePtr _pointer;
		SLTypePtr _reference;
		SLTypePtr _pointed_to_or_referenced_type;
		
		SLFunctionSignature _function_sig;		

		SLStructDefinition _struct_def;
};
