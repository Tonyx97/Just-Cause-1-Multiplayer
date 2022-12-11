#include <defs/standard.h>

#include <mp/net.h>

#include "nh_player_client.h"

#include <shared_mp/player_client/player_client.h>

#include <resource_sys/resource_system.h>

#include <script/lua_ctx/script_objects.h>

#ifdef JC_CLIENT
#include <mp/chat/chat.h>

#include <core/ui.h>
#endif

PacketResult nh::player_client::init(const Packet& p)
{
#ifdef JC_CLIENT
	g_net->add_local(p.get<NID>());
	g_net->set_initialized(true);

	const auto local = g_net->get_local();
	const auto localplayer = local->get_player();

	log(GREEN, "Init packet received (Local NID: {:x})", local->get_nid());
	log(GREEN, "Localplayer PC: {} ({})", (void*)local, localplayer->get_nick());
	log(GREEN, "Localplayer: {}", (void*)localplayer);
#elif JC_SERVER
	const auto pc = p.get_pc();

	pc->set_nick(p.get_str());
	pc->send(Packet(PlayerClientPID_Init, ChannelID_PlayerClient, pc->get_nid()), true);

	logt(YELLOW, "Player {:x} initializing (nick: {})", pc->get_nid(), pc->get_nick());
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::join(const Packet& p)
{
#ifdef JC_SERVER
	const auto pc = p.get_pc();
	const auto player = pc->get_player();

	log(PURPLE, "Player with NID {:x} ({}) trying to join", player->get_nid(), player->get_nick());

	check(player, "Trying to join a PlayerClient that has no Player linked");

	pc->set_joined(true);

	log(PURPLE, "Player with NID {:x} ({}) joined", player->get_nid(), player->get_nick());
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::nick(const Packet& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		const auto nick = p.get_str();

		player->set_nick(nick);

		log(YELLOW, "[{}] {} {}", CURR_FN, player->get_nid(), nick);

		return PacketRes_Ok;
	}
#else
	const auto nick = p.get_str();
	const auto pc = p.get_pc();

	pc->set_nick(nick);

	return PacketRes_Ok;
#endif

	return PacketRes_BadArgs;
}

PacketResult nh::player_client::object_instance_sync(const Packet& p)
{
#ifdef JC_CLIENT
	const auto [nid, type] = p.get_nid_and_type();
	const auto action_type = p.get<NetObjectActionSyncType>();
	const auto localplayer = g_net->get_localplayer();
	const auto net_obj = g_net->get_net_object_by_nid(nid);

	switch (action_type)
	{
	case NetObjectActionSyncType_Create:
	{
		switch (type)
		{
		case NetObject_Player:
		{
			PlayerClient* pc = nullptr;

			auto player = net_obj->cast<Player>();

			if (!player)
			{
				pc = g_net->add_player_client(nid);

				player = pc->get_player();
			}
			else pc = player->get_client();

			const auto transform = p.get<TransformPackedTR>();
			const auto hp = p.get_float();
			const auto max_hp = p.get_float();
			const auto just_joined = p.get_bool();

			if (!player->is_spawned())
				player->spawn();

			player->set_transform(transform);
			player->set_hp(hp);
			player->set_max_hp(max_hp);

			// deserialize player stuff such as skin etc

			player->deserialize_derived(&p);

			// if the player just joined then make sure
			// it's joined in this client and call onPlayerJoin event

			if (just_joined)
				pc->set_joined(true);

			break;
		}
		case NetObject_Damageable:
		case NetObject_Blip:
		case NetObject_Vehicle:
		case NetObject_Pickup:
		case NetObject_Grenade:
		{
			const auto transform = p.get<TransformPackedTR>();
			const auto hp = p.get_float();
			const auto max_hp = p.get_float();

			auto object = net_obj;

			// if object is invalid then create it and we will also
			// deserialize the basic info needed to create the object in
			// ObjectLists::spawn_net_object...

			if (!object)
				object = g_net->spawn_net_object(nid, type, TransformTR(transform), &p);

			if (!object->is_spawned())
				object->spawn();

			object->set_hp(hp);
			object->set_max_hp(max_hp);

			// deserialize derived object from NetObject
			
			object->deserialize_derived(&p);

			break;
		}
		}

		break;
	}
	case NetObjectActionSyncType_Hide:
	{
		if (net_obj)
			net_obj->despawn();

		break;
	}
	case NetObjectActionSyncType_Destroy:
	{
		if (type == NetObject_Player)
		{
			// it's possible the server tells us to destroy a player but it wasn't created
			// usually this happens because some problem in remote players initialization
			
			if (const auto player = net_obj->cast<Player>())
			{
				check(localplayer != player, "A localplayer cannot receive its own NID");

				log(YELLOW, "[{}] Destroyed player with NID {:x}", CURR_FN, player->get_nid());

				g_chat->add_chat_msg(FORMATV("{} has left the server", player->get_nick()));
				g_net->remove_player_client(player->get_client());
			}
		}
		else check(false, "non-player net objects cannot be destroyed here");

		break;
	}
	}
#else
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::ownerships(const Packet& p)
{
#ifdef JC_CLIENT
	// when we receive the ownerships packet we want to clear
	// all local ownerships and fill it again with the new
	// ones

	net_object_globals::clear_owned_entities();

	const auto ownerships = p.get<std::vector<NID>>();

	for (auto nid : ownerships)
		if (const auto net_obj = g_net->get_net_object_by_nid(nid))
			net_obj->set_as_owned();

	return PacketRes_Ok;
#endif

	return PacketRes_NotSupported;
}

PacketResult nh::player_client::resource_action(const Packet& p)
{
#ifdef JC_CLIENT
	const auto rsrc_name = p.get_str();
	const auto action_type = p.get_u8();

	ResourceResult result = ResourceResult_Ok;

	switch (action_type)
	{
	case ResourceAction_Start:		result = g_rsrc->start_resource(rsrc_name);		break;
	case ResourceAction_Stop:		result = g_rsrc->stop_resource(rsrc_name);		break;
	case ResourceAction_Restart:	result = g_rsrc->restart_resource(rsrc_name);	break;
	}
#else
	const auto pc = p.get_pc();
	const auto rsrc_name = p.get_str();
	const auto action_type = p.get_u8();

	ResourceResult result = ResourceResult_Ok;

	if (pc->has_acl("owner") || pc->has_acl("admin"))
	{
		switch (action_type)
		{
		case ResourceAction_Start:		result = g_rsrc->start_resource(rsrc_name);		break;
		case ResourceAction_Stop:		result = g_rsrc->stop_resource(rsrc_name);		break;
		case ResourceAction_Restart:	result = g_rsrc->restart_resource(rsrc_name);	break;
		}
}
	else result = ResourceResult_NotAllowed;

	pc->send(Packet(PlayerClientPID_DebugLog, ChannelID_PlayerClient, Resource::RESULT_TO_STRING(result)), true);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::resources_refresh(const Packet& p)
{
#ifdef JC_SERVER
	g_rsrc->refresh();
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::register_user(const Packet& p)
{
#ifdef JC_CLIENT
	const auto ok = p.get_bool();

	if (ok)
		g_chat->add_chat_msg("##00ff00ffAccount registered");
	else g_chat->add_chat_msg("##ff0000ffCould not register account");
#else
	const auto pc = p.get_pc();
	const auto user = p.get_str();
	const auto pwd = p.get_str();

	pc->send(Packet(PlayerClientPID_RegisterUser, ChannelID_PlayerClient, pc->register_user(user, pwd)), true);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::login_user(const Packet& p)
{
#ifdef JC_CLIENT
	const auto ok = p.get_bool();

	if (ok)
		g_chat->add_chat_msg("##00ff00ffLogged in");
	else g_chat->add_chat_msg("##ff0000ffCould not log in");
#else
	const auto pc = p.get_pc();
	const auto user = p.get_str();
	const auto pwd = p.get_str();

	pc->send(Packet(PlayerClientPID_LoginUser, ChannelID_PlayerClient, pc->login_user(user, pwd)), true);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::logout_user(const Packet& p)
{
#ifdef JC_CLIENT
	const auto ok = p.get_bool();

	if (ok)
		g_chat->add_chat_msg("##00ff00ffLogged out");
	else g_chat->add_chat_msg("##ff0000ffCould not log out");
#else
	const auto pc = p.get_pc();

	pc->send(Packet(PlayerClientPID_LogoutUser, ChannelID_PlayerClient, pc->logout_user()), true);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::debug_log(const Packet& p)
{
#ifdef JC_CLIENT
	g_chat->add_chat_msg(p.get_str());

	// todojc - add the message to the debug box
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::debug_enable_admin_panel(const Packet& p)
{
#ifdef JC_CLIENT
	g_ui->enable_admin_panel(p.get_bool());
#else
	const auto pc = p.get_pc();

	if (pc->get_nick() == "Tony") // looooooooool
		g_net->send_broadcast(p);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::trigger_remote_event(const Packet& p)
{
#ifdef JC_SERVER
	const auto pc = p.get_pc();
#endif

	// code is the same for client and server

	const auto event_name = p.get_str();
	const auto args_count = p.get_u8();

	std::vector<std::any> args_list(args_count);

	for (uint8_t i = 0u; i < args_count; ++i)
		switch (const auto type = p.get_u8())
		{
		case LUA_TUSERDATA:
		{
			switch (const auto ud_type = p.get<ScriptClassType>())
			{
			case ScriptClassType_Vec2: args_list[i] = svec2(p.get<vec2>()); break;
			case ScriptClassType_Vec3: args_list[i] = svec3(p.get<vec3>()); break;
			case ScriptClassType_Vec4: args_list[i] = svec4(p.get<vec4>()); break;
			default: log(RED, "Unknown userdata type {} in {}", ud_type, CURR_FN);
			}

			break;
		}
		case LUA_TLIGHTUSERDATA:	args_list[i] = p.get_net_object();		break;
		case LUA_TSTRING:			args_list[i] = p.get_str();				break;
		case LUA_TNUMBER:			args_list[i] = p.get<lua_Number>();		break;
		case LUA_TBOOLEAN:			args_list[i] = p.get<bool>();			break;
		default: log(RED, "Unknown value type type {} in {}", type, CURR_FN);
		}

	g_rsrc->trigger_remote_event(event_name, args_list);

	return PacketRes_Ok;
}