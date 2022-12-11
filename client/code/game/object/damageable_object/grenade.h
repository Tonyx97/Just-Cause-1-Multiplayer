#pragma once

#include "../alive_object/alive_object.h"

namespace jc::grenade
{
	static constexpr uint32_t INSTANCE_SIZE = 0x2D0;

	namespace fn
	{
		static constexpr uint32_t INFLICT_DAMAGE = 0x748490;
	}

	namespace hook
	{
		void enable(bool apply);
	}
}

class PlayerGrenade : public AliveObject
{
public:

	IMPL_OBJECT_TYPE_ID("CPlayerGrenade");

	static shared_ptr<PlayerGrenade> CREATE(Transform* transform);
};