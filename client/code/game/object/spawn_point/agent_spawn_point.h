#pragma once

#include "spawn_point.h"

namespace jc::agent_spawn_point
{
	static constexpr uint32_t TYPE = 0x254; // AgentType*
}

class AgentType
{
public:
	void initialize();
};

class AgentSpawnPoint : public SpawnPoint
{
public:
	static constexpr auto CLASS_NAME() { return "CAgentSpawnPoint"; }
	static constexpr auto CLASS_ID() { return util::hash::JENKINS(CLASS_NAME()); }

	void create_agent_type();
	void set_preset_name(const char* preset_name);
	void set_spawn_name(const char* spawn_name) const;
	void set_agent_type(AgentType* agent_type);

	const char* get_preset_name() const;
	const char* get_spawn_name() const;

	AgentType* get_agent_type();

	vec3 get_position() const;
};