#include <defs/standard.h>

#include "ee_resource.h"

#include "../exported_entity/exported_entity.h"

ExportedEntityResource* ExportedEntityResource::CREATE()
{
	if (auto obj = jc::game::malloc<ExportedEntityResource>(SIZE()))
	{
		jc::this_call(jc::ee_resource::fn::SETUP, obj);

		obj->set_flag(2);

		return obj;
	}

	return nullptr;
}

void ExportedEntityResource::set_flag(uint32_t flag)
{
	jc::this_call<void>(jc::ee_resource::fn::SET_FLAG, this, flag);
}

void ExportedEntityResource::check() const
{
	jc::this_call<void>(jc::ee_resource::fn::CHECK, this);
}

bool ExportedEntityResource::is_loaded() const
{
	const auto ee = get_exported_entity();

	return ee ? ee->is_loaded() : false;
}

ExportedEntity* ExportedEntityResource::get_exported_entity() const
{
	return jc::read<ExportedEntity*>(this, jc::ee_resource::EXPORTED_ENTITY_RESOURCE);
}