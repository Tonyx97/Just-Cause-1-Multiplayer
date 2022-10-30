#pragma once

namespace jc::cam_settings
{
	static constexpr uint32_t _ = 0x0;

	namespace vt
	{
	}

	namespace hook
	{
		void enable(bool apply);
	}
}

class Character;

class CamSettings
{
public:

	void set_character(weak_ptr<Character> v)
	{
		*REF(weak_ptr<Character>*, this, 0x120) = v;
	}
};