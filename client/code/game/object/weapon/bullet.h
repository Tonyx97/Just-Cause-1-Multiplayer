#pragma once

namespace jc::bullet
{
	static constexpr uint32_t DAMAGE		= 0xC; // float
	static constexpr uint32_t POSITION		= 0x54; // vec3
	static constexpr uint32_t DIRECTION		= 0x64; // vec3
	static constexpr uint32_t LAST_POSITION = 0x70; // vec3
	static constexpr uint32_t MAX_RANGE		= 0x94; // float
	static constexpr uint32_t VELOCITY		= 0x9A; // float
	static constexpr uint32_t IS_ALIVE		= 0x9E; // bool

	namespace vt
	{
		static constexpr uint32_t CALCULATE_DROP = 11;
	}

	namespace hook
	{
		void enable(bool apply);
	}
}

class Bullet
{
public:

	void calculate_drop(float delta);
	void set_position(const vec3& v);
	void set_velocity(float v);
	void set_direction(const vec3& v);

	bool is_alive();

	float get_damage();
	float get_max_range();
	float get_velocity();

	vec3 get_position();
	vec3 get_direction();
	vec3 get_last_position();
};