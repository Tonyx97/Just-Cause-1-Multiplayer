#include <defs/standard.h>

#include "script_registering.h"
#include "script_globals.h"

#include <resource_sys/resource_system.h>

#include <mp/net.h>

#if defined(JC_CLIENT)
#include <game/sys/time/time_system.h>
#include <game/sys/world/day_cycle.h>
#elif defined(JC_SERVER)
#include <shared_mp/player_client/player_client.h>
#endif

/*********/
/*********/
/* UTILS */
/*********/
/*********/

/*************/
/*************/
/* FUNCTIONS */
/*************/
/*************/

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

	vm->add_function("setBuildingLightingEnabled", [](bool v) { g_day_cycle->set_night_time_enabled(v); });
	vm->add_function("isBuildingLightingEnabled", []() { return g_day_cycle->is_night_time_enabled(); });
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
#endif

	// resgister shared functions

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

	/* OBJECTS & SPAWNING */

	vm->add_function("getPlayers", []()
	{
		std::vector<Player*> out;

		g_net->for_each_player([&](NID, Player* player) { out.push_back(player); });

		return out;
	});

	/* WORLD */

	vm->add_function("setTimescale", [](float v)
	{
#ifdef JC_CLIENT
		g_time->set_time_scale(v);
#else
		g_net->get_settings().set_time_scale(v);
#endif
	});

	vm->add_function("getTimescale", []()
	{
#ifdef JC_CLIENT
		return g_time->get_time_scale();
#else
		return g_net->get_settings().get_time_scale();
#endif
	});

	vm->add_function("setDayTime", [](float v)
	{
#ifdef JC_CLIENT
		g_day_cycle->set_time(v);
#else
		g_net->get_settings().set_day_time(v);
#endif
	});

	vm->add_function("getDayTime", []()
	{
#ifdef JC_CLIENT
		return g_day_cycle->get_hour();
#else
		return g_net->get_settings().get_day_time();
#endif
	});

	vm->add_function("setDayTimeEnabled", [](bool v)
	{
#ifdef JC_CLIENT
		return g_day_cycle->set_enabled(v);
#else
		return g_net->get_settings().set_day_time_enabled(v);
#endif
	});

	vm->add_function("isDayTimeEnabled", []()
	{
#ifdef JC_CLIENT
		return g_day_cycle->is_enabled();
#else
		return g_net->get_settings().is_day_time_enabled();
#endif
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
	vm->add_global(jc::script::globals::RSRC_PATH, script->get_rsrc_path());
	vm->add_global(jc::script::globals::RSRC_NAME, script->get_rsrc_name());
}