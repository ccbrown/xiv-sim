#include "C3StructDefinition.h"

#include "C3Type.h"

size_t C3StructDefinition::size() const {
	size_t total = 0;
	for (auto& var : _member_vars) { total += var.type->size(); }
	return total;
}
