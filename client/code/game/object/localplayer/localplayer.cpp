#include <defs/standard.h>

#include "localplayer.h"

Character* LocalPlayer::get_character() const
{
	return jc::read<Character*>(this, jc::localplayer::CHARACTER);
}