#include <defs/standard.h>

#include <game/globals.h>

#include "simple_rigid_object.h"

#include "../base/comps/model.h"
#include "../base/comps/physical.h"

SimpleRigidObject* SimpleRigidObject::ALLOC()
{
	return jc::game::malloc<SimpleRigidObject>(jc::simple_rigid_object::INSTANCE_SIZE);
}

ref<SimpleRigidObject> SimpleRigidObject::create(Transform* transform, const std::string& model_name, const std::string& pfx_name)
{
	jc::this_call<ptr>(jc::simple_rigid_object::fn::SETUP, this, model_name.c_str(), pfx_name.c_str(), transform);

	if (const auto physical = get_physical().obj)
	{
		physical->vcall_unk1();
		physical->vcall_unk2();
	}

	if (const auto model = get_model())
		model->get_instance()->remove_flag(1 << 2);

	return ref<SimpleRigidObject>(this, jc::simple_rigid_object::fn::DELETER_FN);
}