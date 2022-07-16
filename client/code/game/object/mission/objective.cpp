#include <defs/standard.h>

#include "objective.h"

#include "../ui/target_indicator.h"

bool Objective::setup(const vec3& position, const u8vec4& color)
{
	object_base_map map {};

	map.insert<object_base_map::Int>(0x15f99fde, 0); // int
	map.insert<object_base_map::Int>(0x168beeb5, 0); // int
	map.insert<object_base_map::Int>(0x2a788cdc, 1); // int
	map.insert<object_base_map::Int>(0x37a8cd43, 1); // int
	map.insert<object_base_map::Int>(0x473a2e9e, 1); // int
	map.insert<object_base_map::Int>(0x525a07d4, 0); // int
	map.insert<object_base_map::Int>(0x6758526d, 1); // int
	map.insert<object_base_map::Int>(0x6788e86d, 1); // int
	map.insert<object_base_map::Int>(0x68c831ad, 1); // int
	map.insert<object_base_map::Int>(0x7b7eb61a, 1); // int
	map.insert<object_base_map::Int>(0xccd9c837, 0); // int
	map.insert<object_base_map::Int>(0xdb5eb582, 1); // int
	map.insert<object_base_map::Int>(0xde0d8dc5, 54000); // int
	map.insert<object_base_map::Int>(ObjectBase::Hash_Relative, 1); // int
	map.insert<object_base_map::Float>(0x3ee0f1b4, 0.00f); // float
	map.insert<object_base_map::String>(0xb284d36a, "icon_objective"); // string

	//map.insert<object_base_map::String>(0x43677192, R"(failed_timetrial)"); // string
	//map.insert<object_base_map::String>(0x20200f3e, R"({60500BE1-D672-4264-AB85-ABBA7CD9DBD1}::go)"); // string
	//map.insert<object_base_map::String>(0x5ec6a145, R"({60500BE1-D672-4264-AB85-ABBA7CD9DBD1}::go)"); // string
	//map.insert<object_base_map::String>(0xa728d18b, R"(rm_tr_11_tbr)"); // string - or SM_Leave_Area_Msg
	//map.insert<object_base_map::String>(0x646ff7d2, R"(prestige2_Rioja SideGovermentHostile p_small)"); // string
	//map.insert<object_base_map::String>(0xb82c81c0, R"(bomb_explode_pulse)"); // string
	//map.insert<object_base_map::String>(0xc3a589bc, R"(dropoffobjectivematrix)"); // string

	map.insert<object_base_map::String>(ObjectBase::Hash_Class, "CObjective"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Desc, "objective_start"); // string
	map.insert<object_base_map::Vec3>(0xcf235620, &position); // vec3

	init_from_map(&map);

	if (auto target_indicator = get_target_indicator())
	{
		target_indicator->set_color(color);

		return true;
	}

	return false;
}

UITargetIndicator* Objective::get_target_indicator() const
{
	return jc::read<UITargetIndicator*>(this, jc::objective::TARGET_INDICATOR);
}