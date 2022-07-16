#pragma once

namespace jc::stance_controller
{
	static constexpr uint32_t MOVEMENT_ID = 0x24;

	namespace fn
	{
		static constexpr uint32_t SET_BODY_STANCE = 0x625750;
		static constexpr uint32_t SET_ARMS_STANCE = 0x744230;
		static constexpr uint32_t GET_MOVEMENT_ID = 0x6FE850;
	}
}

enum BodyStanceID
{
	BodyStance_Jump = 21
};

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