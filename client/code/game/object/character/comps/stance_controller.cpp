#include <defs/standard.h>

#include "stance_controller.h"

#include "../character.h"

int StanceController::get_movement_id() const
{
	return jc::read<int>(this, jc::stance_controller::MOVEMENT_ID);
}

void BodyStanceController::set_stance(uint32_t id)
{
	//jc::hooks::call<jc::character::hook::body_stance::set_stance_t>(this, id);
}

Character* BodyStanceController::get_character() const
{
	return REF(Character*, this, -jc::character::BODY_STANCE_CONTROLLER);
}

void ArmsStanceController::set_stance(uint32_t id)
{
	//jc::hooks::call<jc::character::hook::arms_stance::set_stance_t>(this, id);
}

Character* ArmsStanceController::get_character() const
{
	return REF(Character*, this, -jc::character::ARMS_STANCE_CONTROLLER);
}