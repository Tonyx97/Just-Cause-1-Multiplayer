#include <defs/standard.h>

#include <mp/net.h>

#include "player.h"

#ifdef JC_CLIENT
#include <game/sys/spawn_system.h>
#include <game/object/spawn_point/spawn_point.h>
#include <game/object/character_handle/character_handle.h>
#include <game/object/character/character.h>
#include <game/object/character/comps/stance_controller.h>
#else
#endif

#ifdef JC_CLIENT
Player::Player(PlayerClient* pc, NID nid) : client(pc)
{
	set_nid(nid);
	set_player_client(pc);

	const auto localplayer = g_net->get_localplayer();

	if (localplayer && !localplayer->equal(this))
	{
		handle = g_spawn->spawn_character("female1", { 0.f, 0.f, 0.f });
		
		set_spawned(true);
	}
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
	if (const auto old_handle = std::exchange(handle, nullptr))
		handle->destroy();
#else
#endif
}

void Player::set_tick_info(const TickInfo& v)
{
	tick_info.clear();

	for (int i = 0; i < v.body_stances_count; ++i) set_body_stance_id(v.body_stances_id[i]);
	for (int i = 0; i < v.arms_stances_count; ++i) set_arms_stance_id(v.arms_stances_id[i]);

	set_transform(v.transform);

	log(CYAN, "tick info received from player {:x}", get_nid());
}

bool Player::spawn()
{
	// if it's already spawned then do nothing

	if (is_spawned())
		return false;

#ifdef JC_CLIENT
	// create agent spawn point if this player is not a local one

	log(GREEN, "Sync spawning for player {:x} {}", get_nid(), get_nick());
#else
	g_net->send_broadcast_reliable<ChannelID_Generic>(get_client(), PlayerPID_Spawn, this);

	set_spawned(true);
#endif

	return true;
}

// info getters/setters

void Player::set_transform(const Transform& transform)
{
	tick_info.transform = transform;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->set_transform(transform); });
#endif
}

void Player::set_body_stance_id(uint32_t id)
{
	const auto index = tick_info.body_stances_count++;
	
	check(index < ARRAY_SIZE(tick_info.body_stances_id), "Invalid body stance index: {}", index);

	tick_info.body_stances_id[index] = id;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->get_body_stance()->set_stance(id); });
#endif
}

void Player::set_arms_stance_id(uint32_t id)
{
	const auto index = tick_info.arms_stances_count++;

	check(index < ARRAY_SIZE(tick_info.arms_stances_id), "Invalid arms stance index: {}", index);

	tick_info.arms_stances_id[index] = id;

#ifdef JC_CLIENT
	verify_exec([&](Character* c) { c->get_arms_stance()->set_stance(id); });
#endif
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