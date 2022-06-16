#pragma once

#include "../alive_object/alive_object.h"

namespace jc::damageable_obj
{
	static constexpr uint32_t INSTANCE_SIZE = 0x298;

	namespace fn
	{
		static constexpr uint32_t INITIALIZE_WITH_MAP = 0x7A2210;
	}
}

class DamageableObject : public AliveObject
{
public:
	static constexpr auto CLASS_NAME() { return "CDamageableObject"; }
	static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

	static ref<DamageableObject> CREATE(Transform* transform, const std::string& model_name, const std::string& pfx_name);
};