#include <defs/standard.h>

#include <mp/net.h>

#include "nh_player_client.h"

#include <shared_mp/player_client/player_client.h>

#include <resource_sys/resource_system.h>

#ifdef JC_CLIENT
#include <mp/chat/chat.h>
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

	const auto [nid, type] = p.get_nid_and_type();
	const auto player_net_obj = g_net->get_net_object_by_nid(nid);

	if (player_net_obj)
		return PacketRes_BadArgs;

	pc->set_joined(true);

	log(GREEN, "Player with NID {:x} ({}) joined", player->get_nid(), player->get_nick());
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
			auto player = net_obj->cast<Player>();

			if (!player)
			{
				const auto pc = g_net->add_player_client(nid);

				player = pc->get_player();
			}

			const bool joined = p.get_bool();
			const auto transform = p.get<TransformPackedTR>();
			const auto hp = p.get_float();
			const auto max_hp = p.get_float();
			const auto skin_info = p.get<Player::SkinInfo>();
			const auto skin = p.get_i32();
			const auto nick = p.get_str();

			if (!player->is_spawned())
				player->spawn();

			player->set_transform(transform);
			player->set_hp(hp);
			player->set_max_hp(max_hp);
			player->set_skin(skin, skin_info.cloth_skin, skin_info.head_skin, skin_info.cloth_color, skin_info.props);
			player->set_nick(nick);

			if (joined)
				g_chat->add_chat_msg(FORMATV("{} has joined the server (NID: {:x})", nick, player->get_nid()));

			break;
		}
		case NetObject_Damageable:
		case NetObject_Vehicle:
		case NetObject_Blip:
		{
			const auto object_id = p.get_u16();
			const auto transform = p.get<TransformPackedTR>();
			const auto hp = p.get_float();
			const auto max_hp = p.get_float();

			auto object = net_obj;

			if (!object)
				object = g_net->spawn_net_object(nid, type, object_id, TransformTR(transform));

			if (!object->is_spawned())
				object->spawn();

			object->set_hp(hp);
			object->set_max_hp(max_hp);

			break;
		}
		}

		break;
	}
	case NetObjectActionSyncType_Hide:
	{
		check(net_obj, "Trying to hide an non-existent object");

		net_obj->despawn();

		break;
	}
	case NetObjectActionSyncType_Destroy:
	{
		if (type == NetObject_Player)
		{
			const auto player = net_obj->cast<Player>();

			check(player, "Trying to destroy a player does not exists");
			check(localplayer != player, "A localplayer cannot receive its own NID");

			g_chat->add_chat_msg(FORMATV("{} has left the server", player->get_nick()));
			g_net->remove_player_client(player->get_client());

			log(YELLOW, "[{}] Destroyed player with NID {:x}", CURR_FN, player->get_nid());
		}
		else check(false, "non-player net objects cannot be destroyed here");

		break;
	}
	}
#else
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::resource_action(const Packet& p)
{
#ifdef JC_CLIENT
	const auto rsrc_name = p.get_str();
	const auto action_type = p.get_u8();

	ResourceResult result = ResourceResult_Ok;

	switch (action_type)
	{
	case ResourceResult_Start:		result = g_rsrc->start_resource(rsrc_name);		break;
	case ResourceResult_Stop:		result = g_rsrc->stop_resource(rsrc_name);		break;
	case ResourceResult_Restart:	result = g_rsrc->restart_resource(rsrc_name);	break;
	}
#else
	const auto pc = p.get_pc();
	const auto rsrc_name = p.get_str();
	const auto action_type = p.get_u8();

	ResourceResult result = ResourceResult_Ok;

	switch (action_type)
	{
	case ResourceResult_Start:		result = g_rsrc->start_resource(rsrc_name);		break;
	case ResourceResult_Stop:		result = g_rsrc->stop_resource(rsrc_name);		break;
	case ResourceResult_Restart:	result = g_rsrc->restart_resource(rsrc_name);	break;
	}

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
#else
	const auto pc = p.get_pc();
	const auto user = p.get_str();
	const auto pwd = p.get_str();

	log(RED, "register {} {}", user, pwd);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::login_user(const Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();
	const auto user = p.get_str();
	const auto pwd = p.get_str();

	log(RED, "login {} {}", user, pwd);
#endif

	return PacketRes_Ok;
}

PacketResult nh::player_client::logout_user(const Packet& p)
{
#ifdef JC_CLIENT
#else
	const auto pc = p.get_pc();

	log(RED, "logout");
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