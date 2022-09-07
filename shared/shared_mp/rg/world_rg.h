#pragma once

#include <librg/librg.h>

class WorldRg;
class EntityRg;

using world_rg_fn = std::function<void(WorldRg*, librg_event*)>;

class WorldRg
{
private:

	librg_world* world = nullptr;

	std::unordered_set<EntityRg*> entities;

	world_rg_fn create_fn = nullptr,
				update_fn = nullptr,
				remove_fn = nullptr;

public:

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
	void call_on_create(librg_event* e) { create_fn(this, e); }
	void call_on_update(librg_event* e) { update_fn(this, e); }
	void call_on_remove(librg_event* e) { remove_fn(this, e); }

	librg_world* get_world() const { return world; }

	EntityRg* add_entity(NID nid);

	bool remove_entity(EntityRg* entity);

	librg_chunk get_chunk_from_position(const vec3& position);

	librg_chunk get_chunk_from_chunk_position(const i16vec3& position);
};