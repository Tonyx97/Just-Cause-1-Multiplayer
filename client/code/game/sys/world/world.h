#pragma once

namespace jc::world
{
	static constexpr uint32_t SINGLETON = 0xD33238; // World*

	static constexpr uint32_t CHARACTER_LIST = 0x20; // std::vector<Character*>
	static constexpr uint32_t LOCALPLAYER	 = 0x30; // LocalPlayer*
}

class Character;
class LocalPlayer;

class World
{
public:
	void init();
	void destroy();

	jc::stl::vector<Character*> get_characters() const;

	LocalPlayer* get_localplayer() const;

	Character* get_localplayer_character() const;
};

inline Singleton<World, jc::world::SINGLETON> g_world;