#pragma once

namespace jc::localplayer
{
	static constexpr uint32_t CHARACTER = 0x1C; // Character*
}

class Character;

class LocalPlayer
{
private:
public:

	Character* get_character() const;
};