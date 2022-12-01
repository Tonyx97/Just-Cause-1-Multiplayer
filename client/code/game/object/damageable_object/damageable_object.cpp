#include <defs/standard.h>

#include <game/transform/transform.h>
#include <game/sys/game/game_control.h>
#include <game/sys/resource/archives.h>

#include "damageable_object.h"

#include <game/sys/resource/physics.h>
#include <game/sys/resource/model_system.h>

shared_ptr<DamageableObject> DamageableObject::CREATE(Transform* transform, const std::string& lod_name, const std::string& pfx_name)
{
	if (auto object = g_game_control->create_object<DamageableObject>())
	{
		// todojc - improve this by properly loading all dependencies such as the 
		// textures etc

		if (!g_model_system->has_resource(lod_name))
			g_model_system->load_rbm(lod_name, g_archives->get_asset_data(lod_name));

		if (!g_physics->has_resource(pfx_name))
			g_physics->load_pfx(pfx_name, g_archives->get_asset_data(pfx_name));

		object_base_map map {};

		auto dummy_mat = mat4(1.f);

		map.insert<object_base_map::Int>(0x5156b18b, 1);
		map.insert<object_base_map::Int>(0x31386da9, 0);
		map.insert<object_base_map::Int>(0x52702583, 0);
		map.insert<object_base_map::Int>(0x10037591, 1);
		map.insert<object_base_map::Int>(0x2c9331bd, 1);
		map.insert<object_base_map::Int>(0xaa49c099, 1);
		map.insert<object_base_map::Int>(0x37595a1a, 1);
		map.insert<object_base_map::Int>(0x8c15bd26, 250);
		map.insert<object_base_map::Int>(0xfdd85bfa, 250);
		map.insert<object_base_map::Int>(0x4b049cc6, 1);
		map.insert<object_base_map::Int>(0x57d204a4, 1);
		map.insert<object_base_map::Int>(0x63171db5, 1);
		map.insert<object_base_map::Int>(0x5753ddd5, 0);
		map.insert<object_base_map::Int>(0xb62fa75a, 0);
		map.insert<object_base_map::Float>(0xac8b94a8, 5000.00f);
		map.insert<object_base_map::Float>(0x43a3aff3, 50.00f);
		map.insert<object_base_map::Float>(0x77acb84d, 100.00f);
		map.insert<object_base_map::Float>(0x71103eeb, 0.00f);
		map.insert<object_base_map::Float>(0xe5ff4048, 0.00f);
		map.insert<object_base_map::Float>(0xf847a592, 1.00f);
		map.insert<object_base_map::String>(ObjectBase::Hash_LOD_Model, lod_name);
		map.insert<object_base_map::String>(ObjectBase::Hash_PFX, pfx_name);
		map.insert<object_base_map::String>(ObjectBase::Hash_Desc, R"(Damageable)");
		map.insert<object_base_map::String>(0xf0503757, R"(360_exp_4_1.xml)");
		map.insert<object_base_map::Mat4>(ObjectBase::Hash_Transform, transform);
		map.insert<object_base_map::Mat4>(0xcab9f941, &dummy_mat);

		object->init_from_map(&map);

		return object;
	}

	return {};
}