#include <defs/standard.h>
#include <defs/glm.h>

#include <mp/net.h>

#include "rg.h"

#define DEBUG_RG 0
#define DEBUG_RG_OWNERSHIP 0

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

int32_t WorldRg::on_create(librg_world* w, librg_event* e)
{
	return get_world_rg(w, CURR_FN)->call_on_create(e);
}

int32_t WorldRg::on_update(librg_world* w, librg_event* e)
{
	return get_world_rg(w, CURR_FN)->call_on_update(e);
}

int32_t WorldRg::on_remove(librg_world* w, librg_event* e)
{
	return get_world_rg(w, CURR_FN)->call_on_remove(e);
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

	write_buffer_size = DEFAULT_WRITE_BUFFER_SIZE();

	write_buffer = BITCAST(char*, malloc(write_buffer_size));

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
#if DEBUG_RG
	log(RED, "---------------------");
#endif

	for (const auto& [id, entity] : entities)
	{
		int32_t result = 0;

		do {
			size_t size = write_buffer_size;

			// use radius 1 because it's very common to stay in the border of a chunk and, if
			// an entity goes to the closest chunk, the distance between observer and entity will
			// be very small yet causing the entity to disappear in the observer's view, so using 1-N as
			// chunk radius should fix this

			result = librg_world_write(world, id, RG_WORLD_CHUNK_RADIUS, write_buffer, &size, nullptr);

			check(result != LIBRG_WORLD_INVALID, "Invalid world");
			check(result != LIBRG_OWNER_INVALID, "Invalid owner");

			if (result > write_buffer_size)
			{
				write_buffer_size += result;

				write_buffer = BITCAST(char*, realloc(write_buffer, write_buffer_size));
			}

		} while (result != LIBRG_OK);

		// refresh the chunk according to the position of the entity

		entity->update_chunk();
	}
}

NetObject* WorldRg::get_event_owner(librg_event* e) const
{
	const auto id = librg_event_owner_get(world, e);

	if (id == LIBRG_EVENT_INVALID)
		return nullptr;

	const auto entity = BITCAST(EntityRg*, librg_entity_userdata_get(world, id));

	return entity ? entity->get_net_obj() : nullptr;
}

NetObject* WorldRg::get_event_entity(librg_event* e) const
{
	const auto id = librg_event_entity_get(world, e);

	if (id == LIBRG_EVENT_INVALID)
		return nullptr;

	const auto entity = BITCAST(EntityRg*, librg_entity_userdata_get(world, id));

	return entity ? entity->get_net_obj() : nullptr;
}

EntityRg* WorldRg::add_entity(NetObject* net_obj, int64_t id)
{
	const auto entity = JC_ALLOC(EntityRg, this, net_obj, id);

	if (!entity)
		return nullptr;

	entities.insert({ id, entity });

	return entity;
}

EntityRg* WorldRg::get_entity_by_id(int64_t id)
{
	auto it = entities.find(id);
	return it != entities.end() ? it->second : nullptr;
}

bool WorldRg::remove_entity(EntityRg* entity)
{
	const auto id = entity->get_id();

	const auto it = entities.find(id);
	if (it == entities.end())
		return false;

	entities.erase(it);

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

u16vec3 WorldRg::get_chunk_size() const
{
	u16vec3 out;

	return librg_config_chunksize_get(world, &out.x, &out.y, &out.z) == LIBRG_OK ? out : u16vec3 {};
}

EntityRg::EntityRg(WorldRg* world, NetObject* net_obj, int64_t id) : world(world), net_object(net_obj), id(id)
{
	const auto world_rg = world->get_world();

	librg_entity_track(world_rg, id);
	librg_entity_owner_set(world_rg, id, id);
	librg_entity_chunk_set(world_rg, id, 0);
	librg_entity_userdata_set(world_rg, id, this);
}

EntityRg::~EntityRg()
{
	librg_entity_untrack(world->get_world(), id);
}

void EntityRg::set_chunk(librg_chunk chunk)
{
	librg_entity_chunk_set(world->get_world(), id, chunk);
}

void EntityRg::set_owner(EntityRg* owner)
{
	if (!owner)
	{
		if (get_owner() == this)
			return;

		librg_entity_owner_set(world->get_world(), id, id);

#if DEBUG_RG_OWNERSHIP
		log(RED, "reset owner");
#endif
	}
	else if (get_owner_id() != owner->id)
	{
		librg_entity_owner_set(world->get_world(), id, owner->id);

#if DEBUG_RG_OWNERSHIP
		log(RED, "new owner {:x}", owner->id);
#endif
	}
}

void EntityRg::update_chunk()
{
	const auto& position = net_object->get_position();

	const auto old_chunk = get_chunk();

	if (const auto new_chunk = world->get_chunk_from_position(position); new_chunk != LIBRG_CHUNK_INVALID && old_chunk != new_chunk)
		set_chunk(new_chunk);

#if DEBUG_RG
	log(GREEN, "Entity {:x} chunk: {} ({:.2f} {:.2f} {:.2f})", net_object->get_nid(), get_chunk(), position.x, position.y, position.z);
#endif
}

bool EntityRg::get_owned_entities(std::vector<NID>& out)
{
	check(net_object->cast<Player>(), "Cannot get owned entities of a non-player object");

	size_t size = (sizeof(owned_entities_buffer) / sizeof(*owned_entities_buffer));

	const auto res = librg_world_fetch_owner(world->get_world(), id, owned_entities_buffer, &size);
	
	if (res != LIBRG_OK)
		return false;

	// the buffer will always contain the current entity (this player)
	// so we will subtract the count by 1 to remove it from the vector
	// we will fill next
	
	out.resize(size - 1ull);

	for (size_t i = 0, out_i = 0; i < size; ++i)
		if (const auto entity_id = owned_entities_buffer[i]; entity_id != id)
			out[out_i++] = static_cast<NID>(entity_id);

	return true;
}

const EntityRg* EntityRg::get_owner() const
{
	const auto owner_id = get_owner_id();

	if (owner_id == LIBRG_WORLD_INVALID ||
		owner_id == LIBRG_ENTITY_UNTRACKED)
		return nullptr;

	if (owner_id == id)
		return this;

	return world->get_entity_by_id(owner_id);
}

int64_t EntityRg::get_owner_id() const
{
	return librg_entity_owner_get(world->get_world(), id);
}

librg_chunk EntityRg::get_chunk() const
{
	return librg_entity_chunk_get(world->get_world(), id);
}