#pragma once

namespace jc::pfx_collision
{
	static constexpr uint32_t UNK = 0x30;
	static constexpr uint32_t PFX = 0x4;

	namespace hook
	{
		void enable(bool apply);
	}
}

class PfxBase;

class PfxCollision
{
private:
public:

	PfxBase* get_pfx() const;
};