#pragma once

#include "../simple_rigid_object/simple_rigid_object.h"

namespace jc::ladder
{
	namespace fn
	{
		static constexpr uint32_t CREATE = 0x666BB0;
	}
}

class Ladder : public SimpleRigidObject
{
public:

	IMPL_OBJECT_TYPE_ID("CLadder");

	Ladder();
	~Ladder();
};