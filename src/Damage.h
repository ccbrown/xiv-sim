#pragma once

enum DamageType {
	DamageTypeUnknown,
	DamageTypeBlunt,
	DamageTypeMagic,
};

struct Damage {
	double amount = 0;
	DamageType type = DamageTypeUnknown;
	bool isCritical = false;
};