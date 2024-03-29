#include <defs/standard.h>

#include "map_icon.h"

#include "../vars/ui.h"

bool UIMapIcon::setup(const std::string& name, const vec3& position)
{
	object_base_map map {};

	map.insert<object_base_map::Int>(0x2414a266, 1); // int
	map.insert<object_base_map::Int>(0x525a07d4, 1); // int
	map.insert<object_base_map::Int>(ObjectBase::Hash_Enabled, 1); // int - enabled
	map.insert<object_base_map::Int>(ObjectBase::Hash_Relative, 0); // int
	map.insert<object_base_map::String>(0x6b7d9fdd, "CGuiMapIcon1"); // string
	//map.insert<object_base_map::String>(0x884ed8bb, R"(A2M08_start_fmv_briefing)"); // string
	map.insert<object_base_map::String>(0xccd9c837, name); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Class, "CGuiMapIcon"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Desc, "UIMapIcon"); // string
	map.insert<object_base_map::Vec3>(0xcf235620, &position); // vec3

	init_from_map(&map);

	return true;
}

void UIMapIcon::set_position(const vec3& v)
{
	jc::write(jc::game::to_u16vec3(v), this, jc::ui::map_icon::POSITION);
}

vec3 UIMapIcon::get_position() const
{
	return jc::game::to_vec3(jc::read<u16vec3>(this, jc::ui::map_icon::POSITION));
}