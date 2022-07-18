#include <defs/standard.h>

#include "nh_player_client.h"

#include <mp/net.h>

#include <shared_mp/player_client/player_client.h>

enet::PacketResult nh::player_client::init(const enet::Packet& p)
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

enet::PacketResult nh::player_client::join(const enet::Packet& p)
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

enet::PacketResult nh::player_client::quit(const enet::Packet& p)
{
#ifdef JC_CLIENT
	if (const auto player = p.get_net_object<Player>())
	{
		check(g_net->get_local()->get_nid() != player->get_nid(), "A localplayer cannot receive 'connect' packet with its NID");
		check(player, "Player must exist before '{}' packet", CURR_FN);

		const auto nid = player->get_nid();
	
		g_net->remove_player_client(player->get_client());

		log(YELLOW, "[{}] Destroyed player with NID {:x}", CURR_FN, nid);

		return enet::PacketRes_Ok;
	}
#endif

	return enet::PacketRes_BadArgs;
}

enet::PacketResult nh::player_client::sync_instances(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto localplayer = g_net->get_localplayer();
	const auto info = p.get<PlayerClientSyncInstancesPacket>();

	log(YELLOW, "Syncing {} player instances...", info.net_objects.size());

	for (const auto& [nid, type] : info.net_objects)
	{
		check(type == NetObject_Player, "Type must be NetObject_Player");

		// we skip the sync of this net object if:
		// 
		// 1. we received our own local player then we skip it,
		// we don't want to do anything to our local player from here
		// 
		// 2. instance already exists then we skip this
		// current net object

		if (localplayer->equal(nid) || g_net->get_net_object_by_nid(nid))
			continue;

		const auto new_pc = g_net->add_player_client(nid);

		::check(new_pc, "Could not create new player");

		const auto player = new_pc->get_player();

		player->spawn();

		log(PURPLE, "Created new player with NID {:x}", player->get_nid());
	}

	log(YELLOW, "All player instances synced (a total of {})", info.net_objects.size());
#elif defined(JC_SERVER)
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::static_info(const enet::Packet& p)
{
#ifdef JC_CLIENT
	const auto localplayer = g_net->get_localplayer();
	const auto info = p.get<PlayerClientStaticInfoPacket>();

	log(YELLOW, "Updating {} player static info...", info.info.size());

	for (const auto& [player, _info] : info.info)
	{
		check(player->get_type() == NetObject_Player, "Type must be NetObject_Player");

		// if this is the localplayer then we skip,
		// we already know our local info

		const auto nid = player->get_nid();

		if (localplayer->equal(nid))
			continue;

		player->set_nick(_info.nick);
		player->set_skin(_info.skin);

		log(PURPLE, "Updated static info for player with NID {:x} ({} - {})", player->get_nid(), player->get_nick(), player->get_skin());
	}
#else
#endif

	return enet::PacketRes_Ok;
}

enet::PacketResult nh::player_client::nick(const enet::Packet& p)
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
	const auto nick = p.get_str();
	const auto pc = p.get_pc();

	pc->set_nick(nick);

	return enet::PacketRes_Ok;
#endif

	return enet::PacketRes_BadArgs;
}