#pragma once

namespace jc::stance_controller
{
	static constexpr uint32_t MOVEMENT_ID = 0x24;

	namespace fn
	{
		static constexpr uint32_t GET_MOVEMENT_ID = 0x6FE850;
	}
}

class StanceController
{
public:

	int get_movement_id() const;
};