#pragma once

namespace jc::spawn_system
{
	static constexpr uint32_t SINGLETON = 0xD32C54; // FactorySystem*

	static constexpr uint32_t MAX_CHARACTER_SPAWNS = 0x3C; // int16
	static constexpr uint32_t MAX_VEHICLE_SPAWNS   = 0x3E; // int16
}

class CharacterHandle;
class SimpleRigidObject;
class AnimatedRigidObject;
class DamageableObject;
class Vehicle;
class AgentSpawnPoint;
class VehicleSpawnPoint;
class MountedGun;
class Ladder;
class ItemPickup;
class UIMapIcon;
class UIMapIconType;
class Objective;
class TrafficLight;
class SoundGameObject;

/*
* Factory/SpawnSystem to create and spawn game objects
*/
class FactorySystem
{
private:
public:

	void init();
	void destroy();
	void set_max_character_spawns(int v);
	void set_max_vehicle_spawns(int v);
	void destroy_character_handle(CharacterHandle* v);
	void destroy_damageable_object(DamageableObject* v);
	void destroy_vehicle(Vehicle* v);
	void destroy_agent_spawn_point(AgentSpawnPoint* v);
	void destroy_vehicle_spawn_point(VehicleSpawnPoint* v);
	void destroy_map_icon(UIMapIcon* v);

	int16_t get_max_character_spawns() const;
	int16_t get_max_vehicle_spawns() const;

	CharacterHandle*		spawn_character(const std::string& model_name, const vec3& position = {}, int weapon_id = 0);
	SimpleRigidObject*		spawn_simple_rigid_object(const vec3& position, const std::string& model_name, const std::string& pfx_name);
	DamageableObject*		spawn_damageable_object(const vec3& position, const std::string& model_name, const std::string& pfx_name);
	Vehicle*				spawn_vehicle(int32_t id, const Transform& transform);
	AgentSpawnPoint*		create_agent_spawn_point(const vec3& position);
	VehicleSpawnPoint*		create_vehicle_spawn_point(const vec3& position, int32_t id, int32_t faction);
	MountedGun*				spawn_mounted_gun(const vec3& position);
	Ladder*					spawn_ladder(const vec3& position, const std::string& model, float length = 10.f);
	ItemPickup*				spawn_general_item_pickup(const vec3& position, uint32_t type, const std::string& model, const std::string& description = {});
	ItemPickup*				spawn_weapon_item_pickup(const vec3& position, uint32_t weapon_id, const std::string& description = {});
	AnimatedRigidObject*	spawn_animated_rigid_object(const vec3& position, const std::string& model, const std::string& pfx_name);
	UIMapIcon*				create_map_icon(const std::string& name, const vec3& position);
	UIMapIconType*			create_map_icon_type(const std::string& name, const std::string& texture, const vec2& scale);
	Objective*				create_objective(const vec3& position, const u8vec4& color);
	TrafficLight*			create_traffic_light(const vec3& position);
	SoundGameObject*		create_sound(const vec3& position, const std::string& bank_name, uint32_t sound_id);
};

inline Singleton<FactorySystem, jc::spawn_system::SINGLETON> g_factory;