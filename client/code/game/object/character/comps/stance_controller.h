#pragma once

namespace jc::stance_controller
{
	static constexpr uint32_t MOVEMENT_ID = 0x24;

	namespace fn
	{
		static constexpr uint32_t GET_MOVEMENT_ID = 0x6FE850;
	}
}

class Character;

class StanceController
{
private:
public:

	int get_movement_id() const;
};

class BodyStanceController : public StanceController
{
private:
public:

	Character* get_character() const;
};

class ArmsStanceController : public StanceController
{
private:
public:

	Character* get_character() const;
};