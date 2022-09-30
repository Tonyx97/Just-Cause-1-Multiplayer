#include <defs/standard.h>

#include "resource_streamer.h"

#include <game/sys/game/game_control.h>

#include <game/object/game_resource/ee_resource.h>
#include <game/object/agent_type/vehicle_type.h>

#include <core/task_system/task_system.h>

void ResourceStreamer::init()
{
}

void ResourceStreamer::destroy()
{
}

bool ResourceStreamer::dispatch(uint32_t ms)
{
	const bool res = jc::this_call<bool>(jc::resource_streamer::fn::DISPATCH, this);

	SleepEx(ms, TRUE);

	return res;
}

bool ResourceStreamer::is_blocked() const
{
	return jc::this_call<bool>(jc::resource_streamer::fn::IS_BLOCKED, this);
}

bool ResourceStreamer::can_add_resource() const
{
	return jc::this_call<bool>(jc::resource_streamer::fn::CAN_ADD_RESOURCE, this);
}

bool ResourceStreamer::all_queues_empty() const
{
	return get_pending_queue()->empty() && get_loading_queue()->empty();
}

bool ResourceStreamer::request_agent_ee(int32_t id, const ee_resource_callback_t& callback, bool now)
{
	auto it = jc::vars::exported_entities.find(id);
	if (it == jc::vars::exported_entities.end())
		return false;

	const auto name = it->second;

	jc::stl::string ee_name = name;

	log(YELLOW, "[ResourceStreamer] Requesting Agent EE '{}'", ee_name.c_str());

	auto ee_resource = ExportedEntityResource::CREATE();

	ee_resource->push(ee_name);

	bool ok = false;

	check(!now, "Type of request not supported yet");

	g_task->launch_sync_task([=](Task*)
	{
		dispatch();

		ee_resource->check();

		return ee_resource->is_loaded();
	}, [=](Task*)
	{
		callback(ee_resource, name);

		ee_resource->free();

		return true;
	});

	return true;
}

bool ResourceStreamer::request_vehicle_ee(int32_t id, const ee_resource_callback_t& callback, bool now)
{
	auto it = jc::vars::exported_entities_vehicles.find(id);
	if (it == jc::vars::exported_entities_vehicles.end())
		return false;

	const auto name = it->second;

	jc::stl::string ee_name = name;

	log(YELLOW, "[ResourceStreamer] Requesting Vehicle EE '{}'", ee_name.c_str());

	auto ee_resource = ExportedEntityResource::CREATE();

	ee_resource->push(ee_name);

	if (now)
	{
		while (!ee_resource->is_loaded())
		{
			dispatch();

			g_game_control->dispatch_locations_load();

			ee_resource->check();
		}

		callback(ee_resource, name);

		ee_resource->free();
	}
	else
	{
		g_task->launch_sync_task([=](Task*)
		{
			dispatch();

			ee_resource->check();

			return ee_resource->is_loaded();
		}, [=](Task*)
		{
			callback(ee_resource, name);

			ee_resource->free();

			return true;
		});
	}

	return true;
}

std::deque<void*>* ResourceStreamer::get_pending_queue() const
{
	return REF(std::deque<void*>*, this, jc::resource_streamer::PENDING_QUEUE);
}

std::deque<void*>* ResourceStreamer::get_loading_queue() const
{
	return REF(std::deque<void*>*, this, jc::resource_streamer::LOADING_QUEUE);
}