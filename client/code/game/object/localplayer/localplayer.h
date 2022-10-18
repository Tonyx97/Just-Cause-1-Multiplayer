#pragma once

namespace jc::localplayer
{
	static constexpr uint32_t CHARACTER = 0x1C; // Character*

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

class LocalPlayer
{
private:
public:

	void reset_weapon_belt();
	void teleport_to_closest_safehouse();
	void increase_current_weapon_scope_fov(float factor = 0.5f);
	void decrease_current_weapon_scope_fov(float factor = 0.5f);

	Character* get_character() const;
};