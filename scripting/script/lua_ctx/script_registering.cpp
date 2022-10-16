#include <defs/standard.h>

#include "script_registering.h"
#include "script_globals.h"
#include "script_objects.h"

#include <resource_sys/resource_system.h>

#include <mp/net.h>

#include "../util/script_timer.h"

#if defined(JC_CLIENT)
#include <core/keycode.h>

#include <mp/chat/chat.h>

#include <game/sys/time/time_system.h>
#include <game/sys/world/day_cycle.h>
#include <game/sys/resource/physics.h>

#include <game/object/character/character.h>
#elif defined(JC_SERVER)
#include <shared_mp/player_client/player_client.h>
#endif

#define NET_OBJ_CHECK(obj)								if (!g_net->has_net_object(obj)) return
#define NET_OBJ_CHECK_DO(obj, action, ...)				if (g_net->has_net_object(obj)) obj->action(__VA_ARGS__); else return
#define NET_OBJ_CHECK_RET(obj, on_true, on_false)		return g_net->has_net_object(obj) ? obj->on_true() : on_false
#define NET_OBJ_CHECK_RET_V(obj, on_true, on_false)		return g_net->has_net_object(obj) ? on_true : on_false
#define NET_OBJ_CHECK_RET_EXP(obj, on_true, on_false)	g_net->has_net_object(obj) ? obj->on_true() : on_false

/*********/
/*********/
/* UTILS */
/*********/
/*********/

namespace jc::script::util
{
	template <typename Fn>
	void serialize_event(const std::string& event_name, const luas::variadic_args& va, const Fn& fn)
	{
		auto out = Packet(PlayerClientPID_TriggerRemoteEvent, ChannelID_PlayerClient);

		// serialize the event name to be called in the server

		out.add(event_name);

		// cast to uint8_t since we are not going to send a fucking packet
		// with more than 256 single parameters

		const int args_size = va.size();

		out.add(static_cast<uint8_t>(args_size));

		for (int i = 0; i < args_size; ++i)
		{
			const auto type = static_cast<uint8_t>(va.get_type(i));

			// serialize the data type

			out.add(type);

			switch (type)
			{
			case LUA_TUSERDATA: check(false, "Unsupported userdata type in {}", CURR_FN); break;
			case LUA_TLIGHTUSERDATA:
			{
				const auto userdata = va.get<void*>(i);

				if (const auto net_obj = g_net->get_net_object(BITCAST(NetObject*, userdata)))
					out.add(net_obj);

				break;
			}
			case LUA_TSTRING:	out.add(va.get<std::string>(i));	break;
			case LUA_TNUMBER:	out.add(va.get<lua_Number>(i));		break;
			case LUA_TBOOLEAN:	out.add(va.get<bool>(i));			break;
			}
		}

		fn(out);
	}
}

/*************/
/*************/
/* FUNCTIONS */
/*************/
/*************/

void jc::script::register_functions(Script* script)
{
	using namespace jc::script;
	using namespace jc::script::util;

	const auto vm = script->get_vm();

#if defined(JC_CLIENT)
	// register client functions

	/* EVENTS */

	vm->add_function("triggerServerEvent", [](luas::state& s, const std::string& name, luas::variadic_args va)
	{
		util::serialize_event(name, va, [&](const Packet& p)
		{
			g_net->send(p);
		});
	});

	/* WORLD */

	vm->add_function("setBuildingLightingEnabled", [](bool v)	{ g_day_cycle->set_night_time_enabled(v); });
	vm->add_function("isBuildingLightingEnabled", []()			{ return g_day_cycle->is_night_time_enabled(); });
	
	vm->add_function("setTimescale", [](float v)	{ g_time->set_time_scale(v); });
	vm->add_function("getTimescale", []()			{ return g_time->get_time_scale(); });

	vm->add_function("setDayTime", [](float v)	{ g_day_cycle->set_time(v); });
	vm->add_function("getDayTime", []()			{ return g_day_cycle->get_hour(); });

	vm->add_function("setDayCycleEnabled", [](bool v)	{ g_day_cycle->set_enabled(v); });
	vm->add_function("isDayCycleEnabled", []()			{ return g_day_cycle->is_enabled(); });

	vm->add_function("setPunchForce", [](float v)	{ Character::SET_GLOBAL_PUNCH_DAMAGE(true, v); });
	vm->add_function("getPunchForce", []()			{ return Character::GET_GLOBAL_PUNCH_DAMAGE(true); });

	/* UI */

	vm->add_function("outputChatBox", [](const std::string& str) { g_chat->add_chat_msg(str); });

	/* KEYCODE */

	vm->add_function("isKeyDown", [](int key) { return g_key->is_key_down(key); });
	vm->add_function("isKeyPressed", [](int key) { return g_key->is_key_pressed(key); });
	vm->add_function("isKeyReleased", [](int key) { return g_key->is_key_released(key); });
	vm->add_function("getMouseWheel", []() { return g_key->get_mouse_wheel_value(); });

	/* PHYSICS */

	vm->add_function("setGravity", [](const svec3& v) { g_physics->get_hk_world()->set_gravity(v.obj()); });
	vm->add_function("getGravity", []() { return svec3(g_physics->get_hk_world()->get_gravity()); });
#elif defined(JC_SERVER)
	// register server functions

	/* EVENTS */

	vm->add_function("triggerClientEvent", [](luas::variadic_args va)
	{
		std::unordered_set<PlayerClient*> targets;

		int index = 0;

		switch (va.get_type(index))
		{
		case LUA_TLIGHTUSERDATA:	// only a player specified
		{
			const auto userdata = va.get<void*>(index++);

			if (const auto player = g_net->get_player(BITCAST(NetObject*, userdata)))
				targets.insert(player->get_client());

			break;
		}
		case LUA_TTABLE:			// a table of players specified
		{
			const auto players_list = va.get<std::vector<void*>>(index++);

			for (const auto& userdata : players_list)
				if (const auto player = g_net->get_player(BITCAST(NetObject*, userdata)))
					targets.insert(player->get_client());

			break;
		}
		case LUA_TSTRING:			// if we find the event then no player specified, send to everyone
		{
			g_net->for_each_player_client([&](NID, PlayerClient* pc)
			{
				targets.insert(pc);
			});

			break;
		}
		}

		if (const auto event_name = va.get<std::string>(index); !event_name.empty())
		{
			// move the variadic argument so serialize_event can work with it
			// from where we left it

			va.set_stack_offset(index);

			util::serialize_event(event_name, va, [&](const Packet& p)
			{
				p.create();

				for (auto pc : targets)
					pc->send(p, false);
			});
		}
	});

	/* WORLD */

	vm->add_function("setTimescale", [](float v)	{ g_net->get_settings().set_time_scale(v); });
	vm->add_function("getTimescale", []()			{ return g_net->get_settings().get_time_scale(); });

	vm->add_function("setDayTime", [](float v)	{ g_net->get_settings().set_day_time(v); });
	vm->add_function("getDayTime", []()			{ return g_net->get_settings().get_day_time(); });

	vm->add_function("setDayTimeEnabled", [](bool v)	{ g_net->get_settings().set_day_time_enabled(v); });
	vm->add_function("isDayTimeEnabled", []()			{ return g_net->get_settings().is_day_time_enabled(); });

	vm->add_function("setPunchForce", [](float v)	{ g_net->get_settings().set_punch_force(v); });
	vm->add_function("getPunchForce", []()			{ return g_net->get_settings().get_punch_force(); });

	/* OBJECTS & SPAWNING */

	vm->add_function("destroyObject", [](NetObject* obj) { NET_OBJ_CHECK(obj); g_net->destroy_net_object(obj); });

	vm->add_function("spawnPlayer", [](Player* player, const svec3& pos, luas::variadic_args va)
	{
		NET_OBJ_CHECK(player);

		auto max_hp = player->get_max_hp();
		auto rotation = glm::eulerAngles(player->get_rotation()).y;
		auto skin = player->get_skin();

		switch (va.size())
		{
		case 2: skin = va.get<int32_t>(1); [[fallthrough]];
		case 1: rotation = va.get<float>(0); break;
		}

		player->respawn(pos.obj(), rotation, skin, max_hp, max_hp);
	});

	vm->add_function("createDamageableObject", [](const svec3& pos, const std::string& lod_name, const std::string& pfx_name)
	{
		return g_net->spawn_net_object(SyncType_Distance, NetObject_Damageable, lod_name, pfx_name, TransformTR(pos.obj()));
	});
#endif

	// resgister shared functions

	/* CLASSES */

	vm->register_class<svec2, svec2(float, float)>(
		"vec2",
		luas::property("x", &svec2::set_x, &svec2::get_x),
		luas::property("y", &svec2::set_y, &svec2::get_y));

	vm->register_class<svec3, svec3(float, float, float)>(
		"vec3",
		luas::property("x", &svec3::set_x, &svec3::get_x),
		luas::property("y", &svec3::set_y, &svec3::get_y),
		luas::property("z", &svec3::set_z, &svec3::get_z));

	vm->register_class<svec4, svec4(float, float, float, float)>(
		"vec4",
		luas::property("x", &svec4::set_x, &svec4::get_x),
		luas::property("y", &svec4::set_y, &svec4::get_y),
		luas::property("z", &svec4::set_z, &svec4::get_z),
		luas::property("w", &svec4::set_w, &svec4::get_w));

	/* EVENTS */

	vm->add_function("cancelEvent", []() { g_rsrc->cancel_event(); });

	vm->add_function("addEvent", [](luas::state& s, const std::string& event_name, luas::lua_fn& fn, luas::variadic_args va)
	{
		const auto script = s.get_global_var<Script*>(jc::script::globals::SCRIPT_INSTANCE);
		const bool allow_remote_trigger = va.size() == 1 ? va.get<bool>(0) : false;

		return g_rsrc->add_event(event_name, fn, script, allow_remote_trigger);
	});

	vm->add_function("removeEvent", [](luas::state& s, const std::string& event_name)
	{
		const auto script = s.get_global_var<Script*>(jc::script::globals::SCRIPT_INSTANCE);

		return g_rsrc->remove_event(event_name, script);
	});

	vm->add_function("triggerEvent", [](const std::string& name, luas::variadic_args va)
	{
		return g_rsrc->trigger_non_remote_event(name, va);
	});

	/* OBJECTS */

	vm->add_function("validateObject", [](NetObject* obj) { NET_OBJ_CHECK_RET_V(obj, obj, nullptr); });
	
	vm->add_function("getPlayers", []()
	{
		std::vector<Player*> out;

		g_net->for_each_joined_player([&](NID, Player* player) { out.push_back(player); return true; });

		return out;
	});

	vm->add_function("getRandomPlayer", []() { return g_net->get_random_player(); });
	vm->add_function("getPlayerFromNID", [](NID nid) { return g_net->get_player_by_nid(nid); });

	/* NET OBJECT */
	
	vm->add_function("getObjectNID", [](NetObject* obj) { NET_OBJ_CHECK_RET(obj, get_nid, INVALID_NID); });
	vm->add_function("getObjectType", [](NetObject* obj) { NET_OBJ_CHECK_RET(obj, get_type, NetObject_Invalid); });

	vm->add_function("setObjectPosition", [](NetObject* obj, const svec3& v) { NET_OBJ_CHECK_DO(obj, set_position, v.obj()); SYNC_NET_VAR(obj, transform, true); });
	vm->add_function("getObjectPosition", [](NetObject* obj) { return svec3(NET_OBJ_CHECK_RET_EXP(obj, get_position, vec3{})); });

	vm->add_function("setObjectRotation", [](NetObject* obj, const svec3& v) { NET_OBJ_CHECK_DO(obj, set_rotation, quat(v.obj())); SYNC_NET_VAR(obj, transform, true); });
	vm->add_function("getObjectRotation", [](NetObject* obj) { return svec3(glm::eulerAngles(NET_OBJ_CHECK_RET_EXP(obj, get_rotation, jc::qua::IDENTITY))); });

	vm->add_function("setObjectHealth", [](NetObject* obj, float v) { NET_OBJ_CHECK_DO(obj, set_hp, v); SYNC_NET_VAR(obj, hp, true); });
	vm->add_function("getObjectHealth", [](NetObject* obj) { NET_OBJ_CHECK_RET(obj, get_hp, 0); });

	vm->add_function("setObjectMaxHealth", [](NetObject* obj, float v) { NET_OBJ_CHECK_DO(obj, set_max_hp, v); SYNC_NET_VAR(obj, max_hp, true); });
	vm->add_function("getObjectMaxHealth", [](NetObject* obj) { NET_OBJ_CHECK_RET(obj, get_max_hp, 0); });

	vm->add_function("setObjectVelocity", [](NetObject* obj, const svec3& v) { NET_OBJ_CHECK_DO(obj, set_velocity, v.obj()); SYNC_NET_VAR(obj, velocity, true); });
	vm->add_function("getObjectVelocity", [](NetObject* obj) { return svec3(NET_OBJ_CHECK_RET_EXP(obj, get_velocity, vec3 {})); });

	/* PLAYER */

	vm->add_function("getPlayerName", [](Player* player) { NET_OBJ_CHECK_RET(player, get_nick, ""); });
	vm->add_function("isPlayerDead", [](Player* player) { return !NET_OBJ_CHECK_RET_EXP(player, is_alive, false); });

	vm->add_function("getPlayerFromName", [](const std::string& name, luas::variadic_args va) -> Player*
	{
		const auto search_pred = [&](char x, char y) { return std::tolower(x) == std::tolower(y); };

		Player* out = nullptr;

		const auto partial_search = va.size() == 1 && va.get<bool>(0);
		
		g_net->for_each_joined_player([&](NID, Player* player)
		{
			if (auto player_name = player->get_nick(); std::equal(player_name.begin(), player_name.end(), name.begin(), name.end(), search_pred))
			{
				out = player;
				return false;
			}
			else if (partial_search)
			{
				if (auto it = std::search(player_name.begin(), player_name.end(), name.begin(), name.end(), search_pred); it != player_name.end())
				{
					out = player;
					return false;
				}
			}

			return true;
		});

		return out;
	});

	/* UTILS */

	vm->add_function("setTimer", [](luas::state& s, luas::lua_fn& fn, int interval, int times, const luas::variadic_args& va)
	{
		auto args = va.push_to_any_vector();

		return g_rsrc->add_timer(fn, s.get_global_var<Resource*>(jc::script::globals::RESOURCE), args, interval, times);
	});

	vm->add_function("killTimer", [](luas::state& s, ScriptTimer* v)
	{
		return g_rsrc->kill_timer(v, s.get_global_var<Resource*>(jc::script::globals::RESOURCE));
	});

	vm->add_function("resetTimer", [](ScriptTimer* v) { v->reset(); });

	vm->add_function("getTimerInfo", [](ScriptTimer* v)
	{
		return std::make_tuple(v->get_interval_left(), v->get_times_remaining(), v->get_interval(), v->get_times());
	});
}

/***********/
/***********/
/* GLOBALS */
/***********/
/***********/

void jc::script::register_globals(Script* script)
{
	const auto vm = script->get_vm();

#if defined(JC_CLIENT)
	// register client globals

	vm->add_global(jc::script::globals::LOCALPLAYER, g_net->get_localplayer());
#elif defined(JC_SERVER)
	// reigster server globals
#endif

	// register shared globals

	vm->add_global(jc::script::globals::SCRIPT_INSTANCE, script);
	vm->add_global(jc::script::globals::RESOURCE, script->get_owner());
	vm->add_global(jc::script::globals::RSRC_PATH, script->get_rsrc_path());
	vm->add_global(jc::script::globals::RSRC_NAME, script->get_rsrc_name());
}