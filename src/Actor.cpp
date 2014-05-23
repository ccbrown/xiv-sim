#include "Actor.h"

#include "Model.h"
#include "Rotation.h"

const Action* Actor::act(const Actor* target) const {
	return _configuration->rotation ? _configuration->rotation->nextAction(this, target) : nullptr;
}

Damage Actor::generatedDamage(const Action* action) const {
	return _configuration->model->generatedDamage(action, this);
}

Damage Actor::acceptedDamage(const Damage& incoming) const {
	return _configuration->model->acceptedDamage(incoming, this);
}
