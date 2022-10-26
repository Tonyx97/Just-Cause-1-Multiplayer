#include <defs/standard.h>
#include <defs/rg_defs.h>

#include "world.h"

#include <game/object/game_player/game_player.h>

void World::init()
{
	set_character_update_max_distance(std::sqrtf(RG_WORLD_MAX_SQUARE_DISTANCE_IN_CHUNK));
}

void World::destroy()
{
}

void World::set_localplayer(GamePlayer* v)
{
	jc::write(v, this, jc::world::LOCALPLAYER);
}

void World::set_character_update_max_distance(float v)
{
	jc::write(static_cast<int>(v), this, jc::world::CHARACTER_UPDATE_MAX_DISTANCE);
}

float World::get_character_update_max_distance() const
{
	return static_cast<float>(jc::read<int32_t>(this, jc::world::CHARACTER_UPDATE_MAX_DISTANCE));
}

jc::stl::vector<Character*> World::get_characters() const
{
	return jc::read<jc::stl::vector<Character*>>(this, jc::world::CHARACTER_LIST);
}

GamePlayer* World::get_local() const
{
	return jc::read<GamePlayer*>(this, jc::world::LOCALPLAYER);
}

Character* World::get_local_character() const
{
	if (auto localplayer = get_local())
		return localplayer->get_character();

	return nullptr;
}