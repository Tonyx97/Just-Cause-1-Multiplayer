#include <defs/standard.h>

#include <game/transform/transform.h>
#include <game/sys/game_control.h>

#include "agent_spawn_point.h"

void AgentType::initialize()
{
	jc::this_call(0x760750, this, this);
}

void AgentSpawnPoint::create_agent_type()
{
	// crashes.
	typedef void(__thiscall * allocate_agent_type_t)(AgentSpawnPoint*);
	static const auto allocate_agent_type = reinterpret_cast<allocate_agent_type_t>(0x0079A990);
	allocate_agent_type(this);
}

void AgentSpawnPoint::set_preset_name(const char* preset_name)
{
	jc::write(preset_name, this, jc::spawn_point::PRESET_NAME);

	const auto size = strlen(preset_name);

	//jc::write(size, this, jc::spawn_point::PRESET_NAME_LENGTH);
}

void AgentSpawnPoint::set_spawn_name(const char* spawn_name) const
{
	const auto size = strlen(spawn_name);
	memcpy((uint8_t*)this + jc::spawn_point::SPAWN_NAME, spawn_name, size);

	//jc::write(size, this, jc::spawn_point::SPAWN_NAME_LENGTH);
}

void AgentSpawnPoint::set_agent_type(AgentType* agent_type)
{
	jc::write(agent_type, this, jc::agent_spawn_point::TYPE);
}

const char* AgentSpawnPoint::get_preset_name() const
{
	return jc::read<const char*>(this, jc::spawn_point::PRESET_NAME);
}

const char* AgentSpawnPoint::get_spawn_name() const
{
	return reinterpret_cast<const char*>((uint8_t*)this + jc::spawn_point::SPAWN_NAME);
}

Character* AgentSpawnPoint::get_character() const
{
	return jc::read<Character*>(this, jc::agent_spawn_point::CHARACTER);
}

AgentType* AgentSpawnPoint::get_agent_type() const
{
	return jc::read<AgentType*>(this, jc::agent_spawn_point::TYPE);
}

vec3 AgentSpawnPoint::get_position() const
{
	return jc::read<vec3>(this, jc::spawn_point::POSITION);
}