#pragma once

enum DamageType {
	DamageTypeUnknown,
	DamageTypeBlunt,
};

struct Damage {
	double amount = 0;
	DamageType type = DamageTypeUnknown;
	bool isCritical = false;
};