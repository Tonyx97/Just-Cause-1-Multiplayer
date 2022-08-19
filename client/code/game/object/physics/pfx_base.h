#pragma once

namespace jc::pfx_base
{
	static constexpr uint32_t USERDATA	= 0xC;

	namespace vt
	{
		static constexpr uint32_t SET_VELOCITY			= 4;
		static constexpr uint32_t _UNK1					= 11;
		static constexpr uint32_t SET_TRANSFORM			= 18;
		static constexpr uint32_t _UNK2					= 27;
		static constexpr uint32_t GET_VELOCITY			= 42;
	}
}

class PfxBase
{
private:
public:

	void vcall_unk1();
	void vcall_unk2();
	void set_velocity(const vec3& v);
	void set_transform(const Transform& v);

	template <typename T>
	T* get_userdata() { return jc::read<T*>(this, jc::pfx_base::USERDATA); }

	vec3 get_velocity() const;
};