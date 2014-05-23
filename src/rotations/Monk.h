#pragma once

#include "../Action.h"
#include "../Aura.h"
#include "../Rotation.h"

namespace rotations {

class Monk : public Rotation {
	public:
		virtual const Action* nextAction(const Actor* subject, const Actor* target) const override;
		
	private:
		struct RaptorForm : Aura {
			RaptorForm() : Aura("raptor-form") {}
			virtual std::chrono::milliseconds duration() const override { return 10s; }
		};
		
		struct BootshineRear : Action {
			BootshineRear() : Action("bootshine-rear") {
				_subjectAuras.push_back(new RaptorForm());
			}
			virtual int damage() const override { return 150; }
		} _bootshineRear;
};

}