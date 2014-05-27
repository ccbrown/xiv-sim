#pragma once

enum DamageType {
	DamageTypeUnknown,
	DamageTypeBlunt,
	DamageTypePiercing,
	DamageTypeSlashing,
	DamageTypeMagic,
};

struct Damage {
	double amount = 0;
	DamageType type = DamageTypeUnknown;
	bool isCritical = false;
};