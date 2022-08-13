#pragma once

namespace jc::sound_system
{
	static constexpr uint32_t SINGLETON = 0xD32ADC; // SoundSystem*

	static constexpr uint32_t HUD_BANK		 = 0x40; // SoundBank*
	static constexpr uint32_t AMBIENCE_BANK	 = 0x50; // SoundBank*
	static constexpr uint32_t EXPLOSION_BANK = 0x54; // SoundBank*
	static constexpr uint32_t WEAPON_BANK	 = 0x58; // SoundBank*
}

class SoundBank;
class SoundGameObject;

class SoundSystem
{
public:
	void init();
	void destroy();

	SoundBank* get_hud_bank();
	SoundBank* get_ambience_bank();
	SoundBank* get_explosion_bank();
	SoundBank* get_weapon_bank();
	SoundBank* get_sound_bank_by_index(int i);
};

inline Singleton<SoundSystem, jc::sound_system::SINGLETON> g_sound;