#include <defs/standard.h>

#include <game/transform/transform.h>
#include <game/sys/game_control.h>

#include "damageable_object.h"

ref<DamageableObject> DamageableObject::CREATE(Transform* transform, const std::string& model_name, const std::string& pfx_name)
{
	auto rf = g_game_control->create_object<DamageableObject>();

	if (!*rf)
		return {};

	auto dummy_mat = mat4(1.f);

	object_base_map map {};

	// todojc - make some id generator for this shit

	auto temp_trash = "{00000000-0000-0000-0000-00000000000" + std::to_string(rand() % 10) + "}::hide_barrel";

	map.insert<ValueType_String>(0xea402acf, R"(building_blocks\general\oil_barrel_red.lod)");
	map.insert<ValueType_String>(0x5b982501, R"(models\building_blocks\general\oil_barrel.pfx)");
	map.insert<ValueType_String>(0xb8fbd88e, R"(Damageable_Barrel)");
	map.insert<ValueType_Mat4>(0xacaefb1, transform->get_matrix());
	map.insert<ValueType_Bool>(0x5156b18b, true);
	map.insert<ValueType_Float>(0xf847a592, 1.00f);
	map.insert<ValueType_String>(0x96190716, temp_trash);
	map.insert<ValueType_String>(0xde66400, temp_trash);
	map.insert<ValueType_Bool>(0x31386da9, false);
	map.insert<ValueType_Int>(0x52702583, 0);
	map.insert<ValueType_Int>(0x10037591, 1);
	map.insert<ValueType_Int>(0x2c9331bd, 1);
	map.insert<ValueType_Int>(0xaa49c099, 1);
	map.insert<ValueType_Int>(0x37595a1a, 1);
	map.insert<ValueType_Int>(0x8c15bd26, 100);
	map.insert<ValueType_Int>(0xfdd85bfa, 100);
	map.insert<ValueType_Int>(0x4b049cc6, 1);
	map.insert<ValueType_Int>(0x57d204a4, 1);
	map.insert<ValueType_Int>(0x63171db5, 1);
	map.insert<ValueType_Float>(0xac8b94a8, 2000.00f);
	map.insert<ValueType_Float>(0x43a3aff3, 50.00f);
	map.insert<ValueType_Float>(0x77acb84d, 100.00f);
	map.insert<ValueType_Float>(0x71103eeb, 0.00f);
	map.insert<ValueType_Float>(0xe5ff4048, 0.00f);
	map.insert<ValueType_String>(0xf0503757, R"(360_exp_3_1.xml)");
	map.insert<ValueType_Mat4>(0xcab9f941, &dummy_mat);
	map.insert<ValueType_Int>(0x5753ddd5, 0);
	map.insert<ValueType_Int>(0xb62fa75a, 0);

	jc::this_call(jc::damageable_obj::fn::INITIALIZE_WITH_MAP, *rf, &map);

	log(RED, "damageable: {:x}", ptr(*rf));

	return rf;
}