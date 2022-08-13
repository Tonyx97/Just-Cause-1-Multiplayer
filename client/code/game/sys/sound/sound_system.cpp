#include <defs/standard.h>

#include "sound_system.h"

#include <game/sys/game/game_control.h>

#include <game/transform/transform.h>

#include <game/object/sound/sound_game_obj.h>

using namespace jc::sound_system;

void SoundSystem::init()
{
}

void SoundSystem::destroy()
{
}

SoundBank* SoundSystem::get_hud_bank()
{
	return jc::read<SoundBank*>(this, HUD_BANK);
}

SoundBank* SoundSystem::get_ambience_bank()
{
	return jc::read<SoundBank*>(this, AMBIENCE_BANK);
}

SoundBank* SoundSystem::get_explosion_bank()
{
	return jc::read<SoundBank*>(this, EXPLOSION_BANK);
}

SoundBank* SoundSystem::get_weapon_bank()
{
	return jc::read<SoundBank*>(this, WEAPON_BANK);
}

SoundBank* SoundSystem::get_sound_bank_by_index(int i)
{
	return jc::read<SoundBank*>(this, 0x40 + (i * 0x4));
}