#pragma once

namespace jc::spawn_system
{
	static constexpr uint32_t SINGLETON = 0xD32C54; // SpawnSystem*

	static constexpr uint32_t MAX_CHARACTER_SPAWNS = 0x3C; // int16
	static constexpr uint32_t MAX_VEHICLE_SPAWNS   = 0x3E; // int16
}

class CharacterHandle;
class SimpleRigidObject;
class DamageableObject;
class AgentSpawnPoint;
class VehicleSpawnPoint;

class SpawnSystem
{
private:
public:
	void init();
	void destroy();
	void set_max_character_spawns(int v);
	void set_max_vehicle_spawns(int v);

	int16_t get_max_character_spawns() const;
	int16_t get_max_vehicle_spawns() const;

	CharacterHandle*   spawn_character(const std::string& model_name, const vec3& position, int weapon_id = 0);
	SimpleRigidObject* spawn_simple_rigid_object(const vec3& position, const std::string& model_name, const std::string& pfx_name);
	DamageableObject*  spawn_damageable_object(const vec3& position, const std::string& model_name, const std::string& pfx_name);
	AgentSpawnPoint*   create_agent_spawn_point(const vec3& position);
	VehicleSpawnPoint* create_vehicle_spawn_point(const vec3& position);
};

inline SpawnSystem* g_spawn = nullptr;