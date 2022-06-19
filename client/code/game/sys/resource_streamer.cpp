#include <defs/standard.h>

#include "resource_streamer.h"

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

std::deque<void*>* ResourceStreamer::get_pending_queue() const
{
	return REF(std::deque<void*>*, this, jc::resource_streamer::PENDING_QUEUE);
}

std::deque<void*>* ResourceStreamer::get_loading_queue() const
{
	return REF(std::deque<void*>*, this, jc::resource_streamer::LOADING_QUEUE);
}