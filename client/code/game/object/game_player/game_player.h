#pragma once

namespace jc::game_player
{
	static constexpr uint32_t CHARACTER = 0x1C; // Character*
	
	static constexpr uint32_t RIGHT		= 0xF8; // float
	static constexpr uint32_t FORWARD	= 0xFC; // float

	namespace fn
	{
		static constexpr uint32_t RESET_WEAPON_BELT						= 0x4C35E0;
		static constexpr uint32_t TELEPORT_TO_CLOSEST_SAFEHOUSE			= 0x4D0490;
		static constexpr uint32_t SET_CURRENT_WEAPON_SCOPE_FOV			= 0x4D0020;
		static constexpr uint32_t INCREASE_CURRENT_WEAPON_SCOPE_FOV		= 0x4D0000;
		static constexpr uint32_t DECREASE_CURRENT_WEAPON_SCOPE_FOV		= 0x4CFFE0;
		static constexpr uint32_t SWITCH_TO_PREVIOUS_WEAPON				= 0x4CDD80;
		static constexpr uint32_t SWITCH_TO_NEXT_WEAPON					= 0x4CDDF0;
	}

	namespace hook
	{
		void enable(bool apply);
	}
}

class Character;

class GamePlayer
{
private:
public:

	void reset_weapon_belt();
	void teleport_to_closest_safehouse();
	void increase_current_weapon_scope_fov(float factor = 0.5f);
	void decrease_current_weapon_scope_fov(float factor = 0.5f);
	void set_right(float v);
	void set_forward(float v);

	float get_right() const;
	float get_forward() const;

	Character* get_character() const;
};