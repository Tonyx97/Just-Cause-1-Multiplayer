#include <defs/standard.h>

#include "exported_entity.h"

#include "../base/base.h"

bool ExportedEntity::load_class_properties(object_base_map*& map)
{
	jc::stl::string type_name;

	return jc::this_call<bool>(jc::exported_entity::fn::LOAD_CLASS_PROPERTIES, this, &type_name, &map);
}

bool ExportedEntity::take_class_property(std::string* class_name, object_base_map*& map)
{
	jc::stl::string out_class_name;

	const bool ok = jc::this_call<bool>(jc::exported_entity::fn::TAKE_CLASS_PROPERTY, this, &out_class_name, &map);

	*class_name = out_class_name.c_str();

	return ok;
}

bool ExportedEntity::is_loaded() const
{
	return jc::this_call<bool>(jc::exported_entity::fn::IS_LOADED, this);
}