#include <defs/standard.h>

#include "localplayer.h"

#include "../character/character.h"

#include <mp/net.h>

namespace jc::localplayer::hook
{
	DEFINE_HOOK_THISCALL(switch_to_previous_weapon, 0x4CDD80, void, LocalPlayer* localplayer, float delta)
	{
		localplayer->increase_current_weapon_scope_fov(0.3f);
	}

	DEFINE_HOOK_THISCALL(switch_to_next_weapon, 0x4CDDF0, void, LocalPlayer* localplayer, float delta)
	{
		localplayer->decrease_current_weapon_scope_fov(0.3f);
	}

	void enable(bool apply)
	{
		switch_to_previous_weapon_hook.hook(apply);
		switch_to_next_weapon_hook.hook(apply);
	}
}

void LocalPlayer::reset_weapon_belt()
{
	jc::this_call(jc::localplayer::fn::RESET_WEAPON_BELT, this);
}

void LocalPlayer::teleport_to_closest_safehouse()
{
	jc::this_call(jc::localplayer::fn::TELEPORT_TO_CLOSEST_SAFEHOUSE, this);
}

void LocalPlayer::increase_current_weapon_scope_fov(float factor)
{
	jc::this_call(jc::localplayer::fn::INCREASE_CURRENT_WEAPON_SCOPE_FOV, this, factor);
}

void LocalPlayer::decrease_current_weapon_scope_fov(float factor)
{
	jc::this_call(jc::localplayer::fn::DECREASE_CURRENT_WEAPON_SCOPE_FOV, this, factor);
}

Character* LocalPlayer::get_character() const
{
	return jc::read<Character*>(this, jc::localplayer::CHARACTER);
}