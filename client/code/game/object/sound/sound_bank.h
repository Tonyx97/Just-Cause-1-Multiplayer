#pragma once

namespace jc::sound_bank
{
	namespace fn
	{
		static constexpr uint32_t PLAY_HUD_SOUND = 0x4BBA10;
	}
};

class SoundBank
{
public:

	void play(int id, int unk = 0);
};