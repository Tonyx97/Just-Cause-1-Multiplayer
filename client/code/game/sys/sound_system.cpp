#include <defs/standard.h>

#include "sound_system.h"

void SoundSystem::init()
{
}

void SoundSystem::destroy()
{
}

SoundBank* SoundSystem::get_hud_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::HUD_BANK);
}

SoundBank* SoundSystem::get_ambience_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::AMBIENCE_BANK);
}

SoundBank* SoundSystem::get_explosion_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::EXPLOSION_BANK);
}

SoundBank* SoundSystem::get_weapon_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::WEAPON_BANK);
}

SoundBank* SoundSystem::get_sound_bank_by_index(int i)
{
	return jc::read<SoundBank*>(this, 0x40 + (i * 0x4));
}