#include <defs/standard.h>

#include "exported_entity.h"

bool ExportedEntity::is_loaded() const
{
	return jc::this_call<bool>(jc::exported_entity::fn::IS_LOADED, this);
}