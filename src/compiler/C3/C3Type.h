#pragma once

#include <string>
#include <memory>

enum C3TypeType {
	C3TypeTypePointer,
	C3TypeTypeReference,
	C3TypeTypeFunction,
	C3TypeTypeStruct,
	C3TypeTypeAuto,
	C3TypeTypeVoid,
	C3TypeTypeNullPointer,
	C3TypeTypeBool,
	C3TypeTypeInt8,
	C3TypeTypeInt32,
	C3TypeTypeInt64,
	C3TypeTypeDouble,
};

enum C3TypeModifier {
	C3TypeModifierNone     = 0,
	C3TypeModifierUnsigned = (1 << 0),
	C3TypeModifierConstant = (1 << 1),
	C3TypeModifierMask     = (1 << 2) - 1
};

class C3Type;

#include "C3TypePtr.h"
#include "C3FunctionSignature.h"
#include "C3StructDefinition.h"

class C3Type {
	public:
		std::string name() const;
		const std::string& global_name() const { return _global_name; }
		C3TypeType type() const { return _type; }
		size_t size() const;

		C3TypePtr pointed_to_type() const;
		C3TypePtr referenced_type() const;

		const C3FunctionSignature& signature() const;

		bool is_integer() const;
		bool is_floating_point() const;
		bool is_auto() const;

		bool is_constant() const;
		bool is_signed() const;
		bool is_defined() const;
		
		int modifiers() const { return _modifiers; }
		void set_modifiers(int modifiers);

		void define(const C3StructDefinition& definition);
		const C3StructDefinition& struct_definition() const { return _struct_def; }

		bool operator==(const C3Type& right) const;
		bool operator!=(const C3Type& right) const;

		static C3TypePtr PointerType(C3TypePtr type);
		static C3TypePtr ReferenceType(C3TypePtr type);
		static C3TypePtr FunctionType(const C3FunctionSignature& signature);
		static C3TypePtr StructType(const std::string& name, const std::string& global_name, const C3StructDefinition& definition);
		static C3TypePtr ModifiedType(C3TypePtr type, int modifiers);
		static C3TypePtr AutoType();
		static C3TypePtr VoidType();
		static C3TypePtr NullPointerType();
		static C3TypePtr BoolType();
		static C3TypePtr Int8Type();
		static C3TypePtr Int32Type();
		static C3TypePtr Int64Type();
		static C3TypePtr DoubleType();

		static C3TypePtr RemoveReference(C3TypePtr type);

	private:
		C3Type(const std::string& name, const std::string& global_name, C3TypeType type);
		C3Type(const std::string& name, C3TypeType type);
		C3Type(const std::string& name, C3TypeType type, C3TypePtr pointed_to_or_referenced);
		C3Type(const C3FunctionSignature& signature);

		std::string _name;
		std::string _global_name;
		C3TypeType _type;
		
		int _modifiers = 0;

		bool _is_defined = false;

		C3TypePtr _pointer;
		C3TypePtr _reference;
		C3TypePtr _pointed_to_or_referenced_type;
		
		C3FunctionSignature _function_sig;		

		C3StructDefinition _struct_def;
};
