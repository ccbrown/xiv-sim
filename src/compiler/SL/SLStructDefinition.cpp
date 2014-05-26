#include "SLStructDefinition.h"

#include "SLType.h"

size_t SLStructDefinition::size() const {
	size_t total = 0;
	for (auto& var : _member_vars) { total += var.type->size(); }
	return total;
}
