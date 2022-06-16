#pragma once

#include "../base/base.h"

namespace jc::force_pulse
{
	namespace fn
	{
		static constexpr uint32_t CREATE	   = 0x786E20;
		static constexpr uint32_t SET_POSITION = 0x787800;
		static constexpr uint32_t SET_FORCE	   = 0x787860;
		static constexpr uint32_t SET_RADIUS   = 0x787880;
		static constexpr uint32_t SET_DAMAGE   = 0x7878B0;
		static constexpr uint32_t ACTIVATE	   = 0x7880A0;
		static constexpr uint32_t DESTROY	   = 0x787060;
	}
}

class ForcePulse : public ObjectBase
{
public:
	ForcePulse();
	~ForcePulse();

	void set_position(const vec3& v);
	void set_force(float v);
	void set_radius(const vec3& v);
	void set_damage(float v);
	void activate();
};