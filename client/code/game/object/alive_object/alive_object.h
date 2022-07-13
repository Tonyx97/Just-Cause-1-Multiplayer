#pragma once

#include "../base/base.h"

namespace jc::alive_object
{
	static constexpr uint32_t MAX_HEALTH = 0x48;		// float
	static constexpr uint32_t HEALTH	 = 0x4C;		// float
	static constexpr uint32_t FLAGS		 = 0x56;		// uint16_t

	namespace vt
	{
		static constexpr uint32_t SET_HEALTH		= 33;
		static constexpr uint32_t INITIALIZE_MODELS = 34;
	}
}

enum AliveObjectFlag
{
	AliveObjectFlag_Invincible = (1 << 0),
};

class AliveObject : public ObjectBase
{
public:
	void set_alive_flag(uint16_t v);
	void remove_alive_flag(uint16_t v);
	void set_hp(float v);
	void set_max_hp(float v);
	void set_invincible(bool v);

	bool is_alive() const;
	bool is_invincible() const;

	uint16_t get_alive_flags() const;

	float get_real_hp() const;
	float get_max_hp() const;
	float get_hp() const;
};