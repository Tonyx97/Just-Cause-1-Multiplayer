#include <defs/standard.h>
#include <defs/glm.h>

#include <mp/net.h>

#include "rg.h"

namespace world_rg
{
	std::unordered_map<librg_world*, WorldRg*> worlds;

	WorldRg* get_world_rg(librg_world* w, const std::string& from)
	{
		const auto it = worlds.find(w);

		check(it == worlds.end(), "WorldRg not found in {}", from);

		const auto world = it->second;
	}
}

using namespace world_rg;

int32_t WorldRg::on_create(librg_world* w, librg_event* e)
{
	const auto world = get_world_rg(w, CURR_FN);

	world->call_on_create(e);

	/*int64_t owner_id = librg_event_owner_get(w, e);
	int64_t entity_id = librg_event_entity_get(w, e);

	if (owner_id != entity_id)
		printf_s("entity: %d was created for player: %d | owner: %d\n", (int)entity_id, (int)owner_id, (int)librg_entity_owner_get(w, entity_id));*/

	return 0;
}

int32_t WorldRg::on_update(librg_world* w, librg_event* e)
{
	const auto world = get_world_rg(w, CURR_FN);

	world->call_on_update(e);
}

int32_t WorldRg::on_remove(librg_world* w, librg_event* e)
{
	const auto world = get_world_rg(w, CURR_FN);

	world->call_on_remove(e);

	/*int64_t owner_id = librg_event_owner_get(w, e);
	int64_t entity_id = librg_event_entity_get(w, e);

	if (owner_id != entity_id)
		printf_s("entity: %d was removed for player: %d\n", (int)entity_id, (int)owner_id);*/

	return 0;
}

WorldRg::WorldRg(
	const i16vec3& chunks,
	const u16vec3& size,
	const world_rg_fn& create_fn,
	const world_rg_fn& update_fn,
	const world_rg_fn& remove_fn) :
	create_fn(create_fn),
	update_fn(update_fn),
	remove_fn(remove_fn)
{
	world = librg_world_create();

	check(world, "Could not create WorldRg");

	librg_event_set(world, LIBRG_WRITE_CREATE, on_create);
	librg_event_set(world, LIBRG_WRITE_UPDATE, on_update);
	librg_event_set(world, LIBRG_WRITE_REMOVE, on_remove);
	librg_config_chunkoffset_set(world, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID, LIBRG_OFFSET_MID);
	librg_config_chunkamount_set(world, chunks.x, chunks.y, chunks.z);
	librg_config_chunksize_set(world, size.x, size.y, size.z);
}

WorldRg::~WorldRg()
{
	librg_world_destroy(world);
}

EntityRg* WorldRg::add_entity(NID nid)
{
	const auto entity = JC_ALLOC(EntityRg, nid);

	if (!entity)
		return nullptr;

	entities.insert(entity);

	return entity;
}

bool WorldRg::remove_entity(EntityRg* entity)
{
	const auto it = entities.find(entity);
	if (it == entities.end())
		return false;

	entities.erase(it);

	librg_entity_untrack(world, entity->get_id());

	JC_FREE(entity);

	return true;
}

librg_chunk WorldRg::get_chunk_from_position(const vec3& position)
{
	return librg_chunk_from_realpos(
		world,
		static_cast<double>(position.x),
		static_cast<double>(position.y),
		static_cast<double>(position.z));
}

librg_chunk WorldRg::get_chunk_from_chunk_position(const i16vec3& position)
{
	return librg_chunk_from_chunkpos(world, position.x, position.y, position.z);
}

EntityRg::EntityRg(WorldRg* world, NID nid) : world(world), nid(nid)
{
	const auto world_rg = world->get_world();
	const auto id = static_cast<int64_t>(nid);
	const auto chunk = world->get_chunk_from_position({ 0.f, 0.f, 0.f }); // todojc

	printf_s("Chunk for new entity: %i\n", int(chunk));

	librg_entity_track(world_rg, id);
	librg_entity_owner_set(world_rg, id, id);
	librg_entity_chunk_set(world_rg, id, chunk != LIBRG_CHUNK_INVALID ? chunk : 0);
	librg_entity_userdata_set(world_rg, id, this);
}

void EntityRg::set_chunk(librg_chunk chunk)
{
	librg_entity_chunk_set(world->get_world(), get_id(), chunk);
}

librg_chunk EntityRg::get_chunk() const
{
	return librg_entity_chunk_get(world->get_world(), get_id());
}