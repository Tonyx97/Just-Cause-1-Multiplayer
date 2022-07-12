#include <defs/standard.h>

#include "stance_controller.h"

#include "../character.h"

int StanceController::get_movement_id() const
{
	return jc::read<int>(this, jc::stance_controller::MOVEMENT_ID);
}

Character* BodyStanceController::get_character() const
{
	return jc::read<Character*>(this, jc::character::BODY_STANCE_CONTROLLER);
}

Character* ArmsStanceController::get_character() const
{
	return jc::read<Character*>(this, jc::character::ARMS_STANCE_CONTROLLER);
}