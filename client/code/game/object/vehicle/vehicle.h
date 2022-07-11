#pragma once

#include "../action_point_owner/action_point_owner.h"

namespace jc::vehicle
{
	namespace vt
	{
	}
}

enum VehicleFaction
{
	VehFaction_None,
	VehFaction_Agency = 2,
	VehFaction_Military,
	VehFaction_Police,
	VehFaction_Guerrilla,
	VehFaction_BlackHand,
	VehFaction_Montano,
	VehFaction_Rioja
};

class Vehicle : public ActionPointOwner
{
public:

	IMPL_OBJECT_TYPE_ID("CVehicle");
};