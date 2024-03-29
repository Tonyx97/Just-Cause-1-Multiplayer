#include <defs/standard.h>

#include "stance_controller.h"

#include "../character.h"

int StanceController::get_movement_id() const
{
	return jc::read<int>(this, jc::stance_controller::MOVEMENT_ID);
}

float StanceController::get_distance_to_ground() const
{
	return jc::read<float>(this, jc::stance_controller::DISTANCE_TO_GROUND);
}

void BodyStanceController::set_stance(uint32_t id)
{
	jc::this_call(0x625750, this, id);
}

Character* BodyStanceController::get_character() const
{
	return REFB(Character*, this, jc::character::BODY_STANCE_CONTROLLER);
}

void ArmsStanceController::set_stance(uint32_t id)
{
	check(false, "todojc");
}

Character* ArmsStanceController::get_character() const
{
	return REFB(Character*, this, jc::character::ARMS_STANCE_CONTROLLER);
}