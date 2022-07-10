#pragma once

#include "simple_rigid_object.h"

namespace jc::animated_rigid_object
{
	static constexpr uint32_t INSTANCE_SIZE = 0x29C;

	namespace fn
	{
		static constexpr uint32_t SETUP	= 0x780040;
	}
}

class AnimatedRigidObject : public SimpleRigidObject
{
public:

	IMPL_OBJECT_TYPE_ID("CAnimatedRigidObject");

};