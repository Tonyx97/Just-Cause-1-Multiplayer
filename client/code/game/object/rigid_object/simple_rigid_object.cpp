#include <defs/standard.h>

#include <game/globals.h>

#include "simple_rigid_object.h"

#include "../base/comps/model.h"

SimpleRigidObject* SimpleRigidObject::ALLOC()
{
	return jc::game::malloc<SimpleRigidObject>(jc::simple_rigid_object::INSTANCE_SIZE);
}

shared_ptr<SimpleRigidObject> SimpleRigidObject::create(Transform* transform, const std::string& lod_name, const std::string& pfx_name)
{
	jc::this_call<ptr>(jc::simple_rigid_object::fn::SETUP, this, lod_name.c_str(), pfx_name.c_str(), transform);

	if (auto physical = get_pfx(); physical && *physical)
	{
		physical->vcall_unk1();
		physical->vcall_unk2();
	}

	if (const auto model = get_model())
		model->get_instance()->remove_flag(1 << 2);

	return shared_ptr<SimpleRigidObject>(this);
}