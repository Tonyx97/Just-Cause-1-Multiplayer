#include <defs/standard.h>

#include "resource_streamer.h"

#include <game/object/resource/ee_resource.h>

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

bool ResourceStreamer::request_exported_entity(uint32_t id, const ee_resource_callback_t& callback, bool now)
{
	auto it = jc::vars::exported_entities.find(id);
	if (it == jc::vars::exported_entities.end())
		return false;

	const auto& ee_name = it->second;

	log(YELLOW, "[ResourceStreamer] Requesting EE '{}'", ee_name);

	auto ee_resource = ExportedEntityResource::CREATE();

	ee_resource->push(ee_name);

	bool ok = false;

	check(now, "Deferred exported entity request not supported yet"); // todojc - add task system to handle this case

	// we have to make sure all queues are empty because other parts of the code might be loading stuff
	// and we would stall that resource loading with this loop (happens with other loops like this 
	// inside the actual game)
	
	if (all_queues_empty())
	{
		while (!ee_resource->is_loaded())
		{
			dispatch();

			ee_resource->check();
		}

		callback(ee_resource);

		ok = true;
	}

	ee_resource->free();

	return ok;
}

std::deque<void*>* ResourceStreamer::get_pending_queue() const
{
	return REF(std::deque<void*>*, this, jc::resource_streamer::PENDING_QUEUE);
}

std::deque<void*>* ResourceStreamer::get_loading_queue() const
{
	return REF(std::deque<void*>*, this, jc::resource_streamer::LOADING_QUEUE);
}