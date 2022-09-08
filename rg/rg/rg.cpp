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

		check(it != worlds.end(), "WorldRg not found in {}", from);

		return it->second;
	}
}

using namespace world_rg;

int32_t on_create(librg_world* w, librg_event* e)
{
	const auto world = get_world_rg(w, CURR_FN);

	/*int64_t owner_id = librg_event_owner_get(w, e);
	int64_t entity_id = librg_event_entity_get(w, e);

	if (owner_id != entity_id)
		log(WHITE, "entity: {} was created for player: {} | owner: {}\n", (int)entity_id, (int)owner_id, (int)librg_entity_owner_get(w, entity_id));*/

	return world->call_on_create(e);
}

int32_t on_update(librg_world* w, librg_event* e)
{
	const auto world = get_world_rg(w, CURR_FN);

	return world->call_on_update(e);
}

int32_t on_remove(librg_world* w, librg_event* e)
{
	const auto world = get_world_rg(w, CURR_FN);

	/*int64_t owner_id = librg_event_owner_get(w, e);
	int64_t entity_id = librg_event_entity_get(w, e);

	if (owner_id != entity_id)
		printf_s("entity: %d was removed for player: %d\n", (int)entity_id, (int)owner_id);*/

	return world->call_on_remove(e);
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

	write_buffer = BITCAST(char*, malloc(write_buffer_size = DEFAULT_WRITE_BUFFER_SIZE()));

	worlds.insert({ world, this });
}

WorldRg::~WorldRg()
{
	check(entities.empty(), "Entities in WorldRg must be empty when destroying a WorldRg");

	worlds.erase(world);

	free(write_buffer);

	librg_world_destroy(world);
}

void WorldRg::update()
{
	for (const auto entity : entities)
	{
		int32_t result = 0;

		do {
			size_t size = write_buffer_size;

			result = librg_world_write(world, entity->get_id(), 0, write_buffer, &size, nullptr);

			check(result != LIBRG_WORLD_INVALID, "Invalid world");
			check(result != LIBRG_OWNER_INVALID, "Invalid owner");

			if (result > write_buffer_size)
			{
				write_buffer_size += result;

				write_buffer = BITCAST(char*, realloc(write_buffer, write_buffer_size));
			}

		} while (result != LIBRG_OK);
	}
}

EntityRg* WorldRg::add_entity(int64_t id)
{
	const auto entity = JC_ALLOC(EntityRg, this, id);

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

EntityRg::EntityRg(WorldRg* world, int64_t id) : world(world), id(id)
{
	const auto world_rg = world->get_world();
	const auto chunk = world->get_chunk_from_position({ 0.f, 0.f, 0.f }); // todojc

	printf_s("Chunk for new entity: %i\n", int(chunk));

	librg_entity_track(world_rg, id);
	librg_entity_owner_set(world_rg, id, id);
	librg_entity_chunk_set(world_rg, id, chunk != LIBRG_CHUNK_INVALID ? chunk : 0);
	librg_entity_userdata_set(world_rg, id, this);
}

void EntityRg::set_chunk(librg_chunk chunk)
{
	librg_entity_chunk_set(world->get_world(), id, chunk);
}

librg_chunk EntityRg::get_chunk() const
{
	return librg_entity_chunk_get(world->get_world(), id);
}