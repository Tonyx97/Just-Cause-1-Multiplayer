#include <defs/standard.h>

#include "sound_game_obj.h"

#include <game/transform/transform.h>

bool SoundGameObject::setup(const vec3& position, const std::string& bank_name, uint32_t sound_id)
{
	object_base_map map {};

	Transform transform(position);

	map.insert<object_base_map::Int>(SoundGameObject::Hash_SoundID, sound_id); // int
	map.insert<object_base_map::Int>(0x525a07d4, 0); // int
	map.insert<object_base_map::Int>(0xbd63e5b2, -1); // int
	map.insert<object_base_map::Int>(0xee2cc81d, 1); // int
	map.insert<object_base_map::Float>(0x51f09be0, 0.00f); // float
	map.insert<object_base_map::Float>(SoundGameObject::Hash_MaxDistance, 100.f); // float
	map.insert<object_base_map::Float>(0x8dfff466, 0.00f); // float
	map.insert<object_base_map::String>(SoundGameObject::Hash_SAB, bank_name + ".sab"); // string
	map.insert<object_base_map::String>(SoundGameObject::Hash_SOB, bank_name + ".sob"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Desc, "CustomSound"); // string
	map.insert<object_base_map::String>(ObjectBase::Hash_Class, "CSoundGameObject"); // string
	map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, &transform); // mat4

	init_from_map(&map);

	return true;
}