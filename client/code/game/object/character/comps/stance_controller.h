#pragma once

#include <game/shared/stances.h>

namespace jc::stance_controller
{
	static constexpr uint32_t DISTANCE_TO_GROUND	= 0x14;
	static constexpr uint32_t MOVEMENT_ID			= 0x24;

	namespace fn
	{
		static constexpr uint32_t SET_BODY_STANCE = 0x625750;
		static constexpr uint32_t SET_ARMS_STANCE = 0x744230;
		static constexpr uint32_t GET_MOVEMENT_ID = 0x6FE850;
	}
}

class Character;

class StanceController
{
private:
public:

	static float MAX_DISTANCE_PARACHUTE_FALL() { return jc::read<float>(0xAECAF0); }

	int get_movement_id() const;

	float get_distance_to_ground() const;
};

class BodyStanceController : public StanceController
{
private:
public:

	void set_stance(uint32_t id);

	Character* get_character() const;
};

class ArmsStanceController : public StanceController
{
private:
public:

	void set_stance(uint32_t id);

	Character* get_character() const;
};