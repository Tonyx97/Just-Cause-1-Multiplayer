#include <defs/standard.h>

#include "sound_bank.h"

void SoundBank::play(int id, int unk)
{
	jc::this_call(jc::sound_bank::fn::PLAY_HUD_SOUND, this, id, unk);
}