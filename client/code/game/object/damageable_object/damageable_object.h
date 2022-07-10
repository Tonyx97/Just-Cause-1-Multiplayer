#pragma once

#include "../alive_object/alive_object.h"

namespace jc::damageable_obj
{
	static constexpr uint32_t INSTANCE_SIZE = 0x298;

	namespace fn
	{
	}
}

class DamageableObject : public AliveObject
{
public:

	IMPL_OBJECT_TYPE_ID("CDamageableObject");

	static ref<DamageableObject> CREATE(Transform* transform, const std::string& model_name, const std::string& pfx_name);
};