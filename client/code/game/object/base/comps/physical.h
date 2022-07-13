#pragma once

namespace jc::physical
{
	namespace vt
	{
		static constexpr uint32_t _UNK1				= 11;
		static constexpr uint32_t _UNK2				= 27;
		static constexpr uint32_t SET_VELOCITY		= 37;
		static constexpr uint32_t GET_VELOCITY		= 43;
	}
}

class Physical
{
public:
	void vcall_unk1();
	void vcall_unk2();
	void set_velocity(const vec3& v);

	vec3 get_velocity() const;
};