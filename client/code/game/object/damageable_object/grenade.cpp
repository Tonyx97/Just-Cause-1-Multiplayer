#include <defs/standard.h>

#include <game/transform/transform.h>

#include <game/object/weapon/bullet.h>

#include <game/sys/game/game_control.h>

#include <mp/net.h>

#include "grenade.h"

namespace jc::grenade::hook
{
	DEFINE_HOOK_THISCALL(inflict_damage, fn::INFLICT_DAMAGE, bool, AliveObject* object, Bullet* bullet, int unk)
	{
		// same logic as AliveObject hook
		
		if (const auto net_obj = g_net->get_net_object_by_game_object(object))
			if (!net_obj->is_owned())
				return false;

		return inflict_damage_hook(object, bullet, unk);
	}

	void enable(bool apply)
	{
		inflict_damage_hook.hook(apply);
	}
}

shared_ptr<PlayerGrenade> PlayerGrenade::CREATE(Transform* transform)
{
	if (auto object = g_game_control->create_object<PlayerGrenade>())
	{
		object_base_map map {};

		map.insert<object_base_map::Int>(0x2c9331bd, 0); // int
		map.insert<object_base_map::Int>(0x37595a1a, 0); // int
		map.insert<object_base_map::Int>(0x4b049cc6, 0); // int
		map.insert<object_base_map::Int>(0x5753ddd5, 0); // int
		map.insert<object_base_map::Int>(0x57d204a4, 0); // int
		map.insert<object_base_map::Int>(0x63171db5, 0); // int
		map.insert<object_base_map::Int>(0x8c15bd26, 10); // int
		map.insert<object_base_map::Int>(0xaa49c099, 0); // int
		map.insert<object_base_map::Int>(0xfdd85bfa, 10); // int
		map.insert<object_base_map::Float>(0x43a3aff3, 10.00f); // float
		map.insert<object_base_map::Float>(0x71103eeb, 0.00f); // float
		map.insert<object_base_map::Float>(0x77acb84d, 6000.00f); // float
		map.insert<object_base_map::Float>(0xac8b94a8, 10000.00f); // float
		map.insert<object_base_map::Float>(0xe5ff4048, 0.00f); // float
		map.insert<object_base_map::Float>(0xf847a592, 0.00f); // float
		map.insert<object_base_map::String>(0xf0503757, R"(ParticleSystems\360_exp_2_1.xml)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(Player_Grenade)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_LOD_Model, R"(Weapons\WEAP_029_lod1.rbm)"); // string
		map.insert<object_base_map::String>(ObjectBase::Hash_PFX, R"(Models\Weapons\WEAP_029.pfx)"); // string
		map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, transform); // mat4

		object->init_from_map(&map);

		return object;
	}

	return {};
}