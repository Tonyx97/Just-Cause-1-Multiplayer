#pragma once

#include <librg/librg.h>

class WorldRg;
class EntityRg;
class NetObject;

using world_rg_fn = std::function<void(WorldRg*, librg_event*)>;

class WorldRg
{
private:

	std::unordered_set<EntityRg*> entities;

	librg_world* world = nullptr;

	char* write_buffer = nullptr;

	int32_t write_buffer_size = 0;

	world_rg_fn create_fn = nullptr,
				update_fn = nullptr,
				remove_fn = nullptr;

public:

	static constexpr auto DEFAULT_WRITE_BUFFER_SIZE() { return 0x10000; }

	static int32_t on_create(librg_world*, librg_event*);
	static int32_t on_update(librg_world*, librg_event*);
	static int32_t on_remove(librg_world*, librg_event*);

	WorldRg(
		const i16vec3& chunks,
		const u16vec3& size,
		const world_rg_fn& create_fn,
		const world_rg_fn& update_fn,
		const world_rg_fn& remove_fn);

	~WorldRg();

	void update();

	int32_t call_on_create(librg_event* e) { create_fn(this, e); return 0; }
	int32_t call_on_update(librg_event* e) { update_fn(this, e); return 0; }
	int32_t call_on_remove(librg_event* e) { remove_fn(this, e); return 0; }

	librg_world* get_world() const { return world; }

	NetObject* get_event_owner(librg_event* e) const;
	NetObject* get_event_entity(librg_event* e) const;

	EntityRg* add_entity(NetObject* net_obj, int64_t nid);

	bool remove_entity(EntityRg* entity);

	librg_chunk get_chunk_from_position(const vec3& position);

	librg_chunk get_chunk_from_chunk_position(const i16vec3& position);
};

class EntityRg
{
private:

	WorldRg* world = nullptr;

	NetObject* net_object = nullptr;

	int64_t id = 0ll;

public:

	EntityRg(WorldRg* world, NetObject* net_obj, int64_t id);

	void set_chunk(librg_chunk chunk);

	NetObject* get_net_obj() const { return net_object; }

	int64_t get_id() const { return id; }

	librg_chunk get_chunk() const;
};