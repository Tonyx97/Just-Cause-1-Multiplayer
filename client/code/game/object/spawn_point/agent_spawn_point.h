#pragma once

#include "spawn_point.h"

namespace jc::agent_spawn_point
{
	static constexpr uint32_t CHARACTER		= 0xEC; // Character*
	static constexpr uint32_t TYPE			= 0x254; // AgentType*
}

class AgentType
{
public:
	void initialize();
};

class AgentSpawnPoint : public SpawnPoint
{
public:

	IMPL_OBJECT_TYPE_ID("CAgentSpawnPoint");

	void create_agent_type();
	void set_preset_name(const char* preset_name);
	void set_spawn_name(const char* spawn_name) const;
	void set_agent_type(AgentType* agent_type);

	const char* get_preset_name() const;
	const char* get_spawn_name() const;

	Character* get_character() const;

	AgentType* get_agent_type() const;

	vec3 get_position() const;
};