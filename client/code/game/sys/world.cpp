#include <defs/standard.h>

#include "world.h"

#include <game/object/localplayer/localplayer.h>

void World::init()
{
}

void World::destroy()
{
}

jc::stl::vector<Character*> World::get_characters() const
{
	return jc::read<jc::stl::vector<Character*>>(this, jc::world::CHARACTER_LIST);
}

LocalPlayer* World::get_localplayer() const
{
	return jc::read<LocalPlayer*>(this, jc::world::LOCALPLAYER);
}

Character* World::get_localplayer_character() const
{
	if (auto localplayer = get_localplayer())
		return localplayer->get_character();

	return nullptr;
}