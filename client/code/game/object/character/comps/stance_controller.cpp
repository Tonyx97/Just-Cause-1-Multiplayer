#include <defs/standard.h>

#include "stance_controller.h"

int StanceController::get_movement_id() const
{
	return jc::read<int>(this, jc::stance_controller::MOVEMENT_ID);
}