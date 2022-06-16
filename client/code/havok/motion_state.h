#pragma once

namespace jc::hk::motion_state
{
	static constexpr uint32_t ROTATION	  = 0x0;
	static constexpr uint32_t TRANSLATION = 0x30;
}

class hkMotionState
{
private:
public:
	void set_position(const vec3& v, bool absolute = true);

	vec3 get_position() const;
};