#include <defs/standard.h>

#include <game/globals.h>
#include <game/transform/transform.h>

#include "traffic_light.h"

bool TrafficLight::setup(const vec3& position)
{
	object_base_map map {};

	Transform transform(position);

	map.insert<object_base_map::Int>(0x2c9331bd, 1); // int
	map.insert<object_base_map::Int>(0x525a07d4, 0); // int
	map.insert<object_base_map::Int>(0x6f54e9a5, 0); // int
	map.insert<object_base_map::String>(0x644b8a98, R"(building_blocks\city\city_trafficlight_halo_yellow.lod)"); // string
	map.insert<object_base_map::String>(0x8504ba40, R"(building_blocks\city\city_trafficlight_halo_red.lod)"); // string
	map.insert<object_base_map::String>(0xb8737ad4, R"(building_blocks\city\city_trafficlight_halo_green.lod)"); // string
	//map.insert<object_base_map::String>(0x91e04f29, R"({5A1DD5DB-3350-45DA-AAEC-1AD2BBC69DFE}::green)"); // string
	//map.insert<object_base_map::String>(0xd82392a6, R"({5A1DD5DB-3350-45DA-AAEC-1AD2BBC69DFE}::red)"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Class, R"(CTrafficLight)"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(TrafficLight)"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_LOD_Model, R"(building_blocks\city\city_trafficlight.lod)"); // string
	map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &transform); // mat4

	init_from_map(&map);

	return true;
}

void TrafficLight::set_light(uint32_t v)
{
	jc::this_call(jc::traffic_light::fn::SET_LIGHT, this, v);
}