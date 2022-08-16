#include <defs/standard.h>

#include "resource_streamer.h"

#include <game/object/resource/ee_resource.h>

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

bool ResourceStreamer::request_exported_entity(int32_t id, const ee_resource_callback_t& callback, bool vehicle, bool now)
{
	jc::stl::string ee_name;

	if (vehicle)
	{
		auto it = jc::vars::exported_entities_vehicles.find(id);
		if (it == jc::vars::exported_entities_vehicles.end())
			return false;

		ee_name = it->second;
	}
	else
	{
		auto it = jc::vars::exported_entities.find(id);
		if (it == jc::vars::exported_entities.end())
			return false;

		ee_name = it->second;
	}

	log(YELLOW, "[ResourceStreamer] Requesting EE '{}'", ee_name.c_str());

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
		callback(ee_resource);

		ee_resource->free();

		return true;
	});

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