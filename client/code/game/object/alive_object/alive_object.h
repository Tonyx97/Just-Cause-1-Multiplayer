#pragma once

#include "../base/base.h"

namespace jc::alive_object
{
	static constexpr uint32_t MAX_HEALTH = 0x48; // float
	static constexpr uint32_t HEALTH	 = 0x4C; // float

	namespace vt
	{
		static constexpr uint32_t SET_HEALTH		= 33;
		static constexpr uint32_t INITIALIZE_MODELS = 34;
	}
}

class AliveObject : public ObjectBase
{
public:
	void set_hp(float v);
	void set_max_hp(float v);

	bool is_alive() const;

	float get_real_hp() const;
	float get_max_hp() const;
	float get_hp() const;
};