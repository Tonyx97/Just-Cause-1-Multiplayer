#include <defs/standard.h>

#include "exported_entity.h"

#include "../base/base.h"

bool ExportedEntity::load_class_properties(object_base_map*& map)
{
	jc::stl::string type_name;

	const bool ok = jc::this_call<bool>(jc::exported_entity::fn::LOAD_CLASS_PROPERTIES, this, &type_name, &map);

	return ok;
}

bool ExportedEntity::is_loaded() const
{
	return jc::this_call<bool>(jc::exported_entity::fn::IS_LOADED, this);
}