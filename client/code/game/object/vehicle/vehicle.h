#pragma once

#include "../action_point_owner/action_point_owner.h"

namespace jc::vehicle
{
	namespace vt
	{
	}
}

class Vehicle : public ActionPointOwner
{
public:

	IMPL_OBJECT_TYPE_ID("CVehicle");
};