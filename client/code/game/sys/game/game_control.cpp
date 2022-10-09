#include <defs/standard.h>
#include <defs/client_basic.h>

#include "game_control.h"

#include <timer/timer.h>

#include <mp/net.h>

#include <game/object/character/character.h>
#include <game/object/localplayer/localplayer.h>
#include <game/object/character_handle/character_handle.h>
#include <game/sys/world/world.h>
#include <game/sys/core/factory_system.h>
#include <core/task_system/task_system.h>
#include <core/keycode.h>
#include <core/test_units.h>

#include <resource_sys/resource_system.h>

namespace jc::game_control
{
	static inline const std::set<std::string_view> default_blocked_objects =
	{
		// ui

		"CGuiLiberateSettlement",
		"CGuiPdaMap",
		"CGuiExtract",
		"CGuiRadioBtnGrp",
		"CGuiFactions",
		"CGuiAnimatedWindow",
		"CGuiDialog",
		"CGuiActionMap_PC",
		"CGuiCreditsScreen",
		"CTextResource",
		"CGuiMenuWindow",
		"CUserInputObject",
		"CGuiFMVObject",
		"CGuiMapIcon",
		"CGuiMapSwitchIcon",
		"CMusicObject",
		"CGuiErrorMsg",
		"CGuiRouterObject",
		"CGuiTextObject",
		"CGuiSpinner",
		"CInfoMessage",
		"CProvinceInfoMessage",

		// rest

		"CAgentSpawnPoint",
		"CVehicleSpawnPoint",
		"CObjectSpawnPoint",
		"CObjective",
		"CSafeHouse",
		"CGarage",
		"CRaceManager",
		"CItemPickup",
		"CMountedGun",
		"CProvinceSettlement",
		"CSettlementSoundObject",
		"CInterestPoint",
		"CBookMark",
		"CNamedPoint",
		"CNamedArea",
		"CNamedPointFinder",
		"CRoadFileLoader",
		"CDropoffPoint",
		"CSideMissionTarget",
		"CMission",
		"CCollectItemsManager",
		"CGroupSpawner",
		"CSideMissionEmployers",
		"CSideMissionType",
		"CFactionRelationSetter",
		"CPause",
		"CSpawnGroup",
		"CSafeArea",
	};

#if FAST_LOAD
	static constexpr bool enable_block = true;
#else
	static constexpr bool enable_block = false;
#endif

	bool ignore_blocked_objects = false;
}

DEFINE_HOOK_THISCALL(create_object, 0x4EE350, ref<ObjectBase>*, GameControl* gc, ref<ObjectBase>* r, jc::stl::string* class_name, bool enable_now)
{
	if (!jc::game_control::ignore_blocked_objects && jc::game_control::enable_block)
	{
		const auto class_name_str = class_name->c_str();

		if (jc::game_control::default_blocked_objects.contains(class_name_str))
		{
			r->make_invalid();

			return nullptr;
		}

		//log(RED, "created '{}'", class_name_str);
	}

	return create_object_hook(gc, r, class_name, enable_now);
}

void* GameControl::create_object_internal(void* r, jc::stl::string* class_name, bool enable_now)
{
	jc::game_control::ignore_blocked_objects = true;

	const auto res = create_object_hook(this, BITCAST(ref<ObjectBase>*, r), class_name, enable_now);

	jc::game_control::ignore_blocked_objects = false;

	return res;
}

void GameControl::init()
{
}

void GameControl::destroy()
{
}

void GameControl::create_global_objects()
{
	// create some extra icon types

	g_factory->create_map_icon_type("handshake", "hud_handshake.dds", vec2(0.02f));
	g_factory->create_map_icon_type("aim_cross", "aim_new_cross.dds", vec2(0.02f));
	g_factory->create_map_icon_type("black_square", "binoculars_black.dds", vec2(0.02f));
	g_factory->create_map_icon_type("compass_arrow", "hud_compass_arrow.dds", vec2(0.02f));
	g_factory->create_map_icon_type("north", "hud_compass_n.dds", vec2(0.02f));
	g_factory->create_map_icon_type("liberate_bg", "hud_liberate_bg.dds", vec2(0.02f));
	g_factory->create_map_icon_type("liberate_box", "hud_liberate_box.dds", vec2(0.02f));
	g_factory->create_map_icon_type("liberate_house", "hud_liberate_house.dds", vec2(0.025f));
	g_factory->create_map_icon_type("air_veh", "pda_icon_airvehicle.dds", vec2(0.0125f));
	g_factory->create_map_icon_type("air_veh_friendly", "pda_icon_airvehicle_friendly.dds", vec2(0.0125f));
	g_factory->create_map_icon_type("player", "pda_icon_player.dds", vec2(0.02f));
	g_factory->create_map_icon_type("player_blip", "pda_icon_interestpoint_collect.dds", vec2(0.0125f));
	g_factory->create_map_icon_type("aa_gun", "pda_icon_aa_gun.dds", vec2(0.0125f));
}

void GameControl::hook_create_object()
{
	create_object_hook.hook();
}

void GameControl::unhook_create_object()
{
	create_object_hook.unhook();
}

void GameControl::dispatch_locations_load()
{
	jc::this_call(jc::game_control::fn::DISPATCH_LOCATIONS_LOAD, this);
}

void GameControl::on_tick()
{
#ifdef JC_DBG
	// debug test units

	jc::test_units::test_0();
#endif

	// do mod logic

	g_net->tick();
	g_net->update_objects();
	g_rsrc->trigger_event(jc::script::event::ON_TICK);
	g_task->process();

	timer::dispatch();
}