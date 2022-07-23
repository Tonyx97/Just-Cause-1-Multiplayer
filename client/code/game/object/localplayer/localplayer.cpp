#include <defs/standard.h>

#include "localplayer.h"

#include "../character/character.h"

#include <mp/net.h>

void LocalPlayer::respawn()
{
	if (const auto character = get_character())
	{
		// respawn our character

		character->respawn();

		// send respawning packet to the server

		if (const auto local = g_net->get_localplayer())
			local->respawn(character->get_real_hp(), character->get_max_hp());
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

Character* LocalPlayer::get_character() const
{
	return jc::read<Character*>(this, jc::localplayer::CHARACTER);
}