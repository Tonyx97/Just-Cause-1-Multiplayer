#pragma once

namespace jc::pfx_instance
{
	static constexpr uint32_t USERDATA	= 0xC;

	namespace fn
	{
		static constexpr uint32_t CTOR = 0x4D8310;
	}

	namespace vt
	{
		static constexpr uint32_t SET_VELOCITY			= 4;
		static constexpr uint32_t _UNK1					= 11;
		static constexpr uint32_t SET_TRANSFORM			= 18;
		static constexpr uint32_t _UNK2					= 27;
		static constexpr uint32_t GET_VELOCITY			= 42;
	}
}

class PfxInstance
{
protected:

	shared_ptr<void*> instance;
	shared_ptr<void*> prototype;

	bool unk;

public:

	PfxInstance() { jc::this_call(jc::pfx_instance::fn::CTOR, this); }

	virtual void get_aabb() {}

	void vcall_unk1();
	void vcall_unk2();
	void set_velocity(const vec3& v);
	void set_transform(const Transform& v);

	operator bool() const { return !!instance; }

	template <typename T>
	T* as() const { return BITCAST(T*, this); }

	template <typename T>
	T* get_userdata() { return jc::read<T*>(this, jc::pfx_instance::USERDATA); }

	vec3 get_velocity() const;
};