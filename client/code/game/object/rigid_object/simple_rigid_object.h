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

	IMPL_OBJECT_TYPE_ID("CSimpleRigidObject");

	static SimpleRigidObject* ALLOC();

	ref<SimpleRigidObject> create(Transform* transform, const std::string& lod_name, const std::string& pfx_name);
};