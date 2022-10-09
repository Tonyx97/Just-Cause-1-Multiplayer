#pragma once

namespace jc::localplayer
{
	static constexpr uint32_t CHARACTER = 0x1C; // Character*

	namespace fn
	{
		static constexpr uint32_t RESET_WEAPON_BELT					= 0x4C35E0;
		static constexpr uint32_t TELEPORT_TO_CLOSEST_SAFEHOUSE		= 0x4D0490;
	}
}

class Character;

class LocalPlayer
{
private:
public:

	void reset_weapon_belt();
	void teleport_to_closest_safehouse();

	Character* get_character() const;
};