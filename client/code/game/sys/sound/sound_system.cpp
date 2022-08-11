#include <defs/standard.h>

#include "sound_system.h"

#include <game/sys/game/game_control.h>

#include <game/transform/transform.h>

#include <game/object/sound/sound_game_obj.h>

namespace jc::sound_system
{
	namespace v
	{
		std::vector<ref<SoundGameObject>> sound_game_objects;
	}
}

using namespace jc::sound_system::v;

void SoundSystem::init()
{
}

void SoundSystem::destroy()
{
	sound_game_objects.clear();
}

SoundGameObject* SoundSystem::create_sound(const vec3& position, const std::string& bank_name, uint32_t sound_id)
{
	if (auto rf = g_game_control->create_object<SoundGameObject>())
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

		rf->init_from_map(&map);

		return rf.move_to(sound_game_objects);
	}

	return nullptr;
}

SoundBank* SoundSystem::get_hud_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::HUD_BANK);
}

SoundBank* SoundSystem::get_ambience_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::AMBIENCE_BANK);
}

SoundBank* SoundSystem::get_explosion_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::EXPLOSION_BANK);
}

SoundBank* SoundSystem::get_weapon_bank()
{
	return jc::read<SoundBank*>(this, jc::sound_system::WEAPON_BANK);
}

SoundBank* SoundSystem::get_sound_bank_by_index(int i)
{
	return jc::read<SoundBank*>(this, 0x40 + (i * 0x4));
}