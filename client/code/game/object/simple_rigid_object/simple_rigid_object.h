#pragma once

#include "../base/base.h"

namespace jc::simple_rigid_object
{
	static constexpr uint32_t INSTANCE_SIZE = 0xE4;

	namespace fn
	{
		static constexpr uint32_t SETUP		 = 0x795110;
		static constexpr uint32_t DELETER_FN = 0x673E70;
	}
}

class SimpleRigidObject : public ObjectBase
{
public:
	static constexpr auto CLASS_NAME() { return "CSimpleRigidObject"; }
	static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

	static SimpleRigidObject* ALLOC();

	ref<SimpleRigidObject> create(Transform* transform, const std::string& model_name, const std::string& pfx_name);
};