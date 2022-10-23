#pragma once

#include "../alive_object/alive_object.h"

namespace jc::damageable_obj
{
	static constexpr uint32_t INSTANCE_SIZE = 0x298;

	namespace fn
	{
		static constexpr uint32_t SET_TRANSFORM = 0x7A2020;
	}
}

class DamageableObject : public AliveObject
{
public:

	IMPL_OBJECT_TYPE_ID("CDamageableObject");

	static shared_ptr<DamageableObject> CREATE(Transform* transform, const std::string& lod_name, const std::string& pfx_name);
};