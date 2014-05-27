#pragma once

class Action;
class Actor;

class Rotation {
	public:
		virtual ~Rotation() = default;
		
		virtual const Action* nextAction(Actor* subject, const Actor* target) const = 0;
};