#include <defs/standard.h>

#include "nh_player_client.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::player_client::init(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	g_net->add_local(p.get_uint());
	g_net->set_initialized(true);

	const auto local = g_net->get_local();
	const auto localplayer = local->get_player();

	log(GREEN, "Init packet received (Local NID: {:x})", local->get_nid());
	log(GREEN, "Localplayer PC: {} ({})", (void*)local, localplayer->get_nick());
	log(GREEN, "Localplayer: {}", (void*)localplayer);
#elif JC_SERVER
	const auto pc = p.get_pc();

	pc->set_nick(p.get_str());
	pc->send_reliable<ChannelID_PlayerClient>(PlayerClientPID_Init, pc->get_nid());

	logt(YELLOW, "Player {:x} initializing (nick: {})", pc->get_nid(), pc->get_nick());
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::join(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		log(GREEN, "Player with NID {:x} ({}) joined", player->get_nid(), player->get_nick());
	}
#elif JC_SERVER
	// sync net objects instances when this player loads
	// and also sync all players static info and spawning

	p.get_pc()->set_joined(true);
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::quit(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		check(g_net->get_local()->get_nid() != player->get_nid(), "A localplayer cannot receive 'connect' packet with its NID");
		check(player, "Player must exist before '{}' packet", CURR_FN);

		log(YELLOW, "[{}] Destroyed player with NID {:x}", CURR_FN, player->get_nid());
	
		g_net->remove_player_client(player->get_client());

		return enet::PacketRes_Ok;
	}
#endif

	return enet::PacketRes_BadArgs;
}

enet::PacketResult nh::player_client::sync_instances(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	const auto count = p.get_int();
	const auto localplayer = g_net->get_localplayer();

	log(YELLOW, "Syncing {} player instances...", count);

	for (int i = 0; i < count; ++i)
	{
		DESERIALIZE_NID_AND_TYPE(p);

		// we skip the sync of this net object if:
		// 
		// 1. we received our own local player then we skip it,
		// we don't want to do anything to our local player from here
		// 
		// 2. instance already exists then we skip this
		// current net object

		const bool skip = localplayer->equal(nid) ||
						  g_net->get_net_object_by_nid(nid);

		check(type == NetObject_Player, "Type must be NetObject_Player");

		if (skip)
			p.skip<PacketCheck_PlayerStaticInfo>();
		else
		{
			const auto new_pc = g_net->add_player_client(nid);

			::check(new_pc, "Could not create new player");

			const auto player = new_pc->get_player();
			const auto info = p.get_struct<PacketCheck_PlayerStaticInfo>();

			player->set_nick(*info.nick);
			player->set_skin(info.skin);
			player->spawn();

			log(PURPLE, "Created new player with NID {:x} ({} - {})", player->get_nid(), player->get_nick(), player->get_skin());
		}
	}

	log(YELLOW, "All player instances synced (a total of {})", count);
#elif defined(JC_SERVER)
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::nick(const enet::PacketR& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		const auto nick = p.get_str();

		player->get_client()->set_nick(nick);

		log(YELLOW, "[{}] {} {}", CURR_FN, player->get_nid(), nick);

		return enet::PacketRes_Ok;
	}
#else
	const auto nick = p.get_str<std::string>();
	const auto pc = p.get_pc();

	pc->set_nick(nick);

	return enet::PacketRes_Ok;
#endif

	return enet::PacketRes_BadArgs;
}