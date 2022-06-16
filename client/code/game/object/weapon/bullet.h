#pragma once

namespace jc::bullet
{
	static constexpr uint32_t DAMAGE		= 0xC; // float
	static constexpr uint32_t POSITION		= 0x54; // vec3
	static constexpr uint32_t DIRECTION		= 0x64; // vec3
	static constexpr uint32_t LAST_POSITION = 0x70; // vec3
	static constexpr uint32_t MAX_RANGE		= 0x94; // float
	static constexpr uint32_t IS_ALIVE		= 0x9E; // bool
}

class Bullet
{
public:
	bool is_alive();

	float get_damage();
	float get_max_range();

	vec3 get_position();
	vec3 get_direction();
	vec3 get_last_position();
};