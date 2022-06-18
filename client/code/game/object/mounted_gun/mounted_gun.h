#pragma once

#include "../action_point_owner/action_point_owner.h"

namespace jc::mounted_gun
{
	static constexpr uint32_t BEGIN_USED = 0x74; // bool
	static constexpr uint32_t CONTROLLER = 0x78; // MountedController* (actually idk)
}

class MountedGun : public ActionPointOwner
{
public:

	static constexpr auto CLASS_NAME() { return "CMountedGun"; }
	static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

	bool is_begin_used() const;
};