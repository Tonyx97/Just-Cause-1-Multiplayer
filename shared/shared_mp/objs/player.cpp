#include <defs/standard.h>

#include <mp/net.h>

#include "player.h"

#ifdef JC_CLIENT
#include <game/sys/spawn_system.h>
#include <game/object/spawn_point/spawn_point.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/character.h>
#else
#endif

#ifdef JC_CLIENT
Player::Player(PlayerClient* pc, NID nid) : client(pc)
{
	set_nid(nid);
	set_player_client(pc);
}

void Player::verify_exec(const std::function<void(Character*)>& fn)
{
	if (!handle)
		return;

	if (const auto character = handle->get_character())
		fn(character);
}
#else
Player::Player(PlayerClient* pc) : client(pc)
{
	set_player_client(pc);
}
#endif

Player::~Player()
{
#ifdef JC_CLIENT
	handle->destroy();
	handle = nullptr;
#else
#endif
}

bool Player::sync_spawn()
{
#ifdef JC_CLIENT
	// create agent spawn point if this player is not a local one

	const auto localplayer = g_net->get_localplayer();

	if (localplayer && !localplayer->equal(this))
		handle = g_spawn->spawn_character("female1", {});

	log(GREEN, "Sync spawning for player {:x} {}", get_nid(), get_nick());
#else
	if (client->is_loaded())
		g_net->send_broadcast_reliable<ChannelID_Generic>(get_client(), PlayerPID_SyncSpawn, this);
#endif

	return true;
}

// info getters/setters

void Player::set_transform(const Transform& transform)
{
	info.transform = transform;

#ifdef JC_CLIENT
	verify_exec([=](Character* c) { c->set_transform(transform); });
#endif
}

Transform Player::get_transform() const
{
	return info.transform;
}

// static info getters/setters

void Player::set_nick(const std::string& v)
{
	static_info.nick = v;
}

void Player::set_skin(uint32_t v)
{
	static_info.skin = v;

#ifdef JC_CLIENT
#endif
}

uint32_t Player::get_skin() const
{
	return static_info.skin;
}