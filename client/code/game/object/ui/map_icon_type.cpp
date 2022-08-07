#include <defs/standard.h>

#include "map_icon_type.h"

#include "../vars/ui.h"

bool UIMapIconType::setup(const std::string& name, const std::string& texture, const vec2& scale)
{
	object_base_map map {};

	map.insert<object_base_map::String>(Hash_TextureName, texture);
	map.insert<object_base_map::String>(Hash_NameID, name);
	map.insert<object_base_map::Float>(Hash_ScaleX, scale.x);
	map.insert<object_base_map::Float>(Hash_ScaleY, scale.y);
	map.insert<object_base_map::Int>(Hash_Unk1, 2);
	map.insert<object_base_map::Int>(Hash_Unk2, 178);

	init_from_map(&map);

	return true;
}