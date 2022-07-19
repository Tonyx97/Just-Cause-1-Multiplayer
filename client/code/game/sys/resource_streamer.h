#pragma once

namespace jc::resource_streamer
{
	static constexpr uint32_t SINGLETON = 0xDA1C24; // ResourceStreamer*

	static constexpr uint32_t PENDING_QUEUE = 0x78; // std::deque<>
	static constexpr uint32_t LOADING_QUEUE	= 0x8C; // std::deque<>

	namespace fn
	{
		static constexpr uint32_t DISPATCH			= 0x989640;
		static constexpr uint32_t IS_BLOCKED		= 0x987FF0;
		static constexpr uint32_t CAN_ADD_RESOURCE  = 0x988010;
	}
}

class Resource;
class ExportedEntityResource;

using resource_callback_t = std::function<void(Resource*)>;
using ee_resource_callback_t = std::function<void(ExportedEntityResource*)>;

class ResourceStreamer
{
public:

	void init();
	void destroy();
	
	bool dispatch(uint32_t ms = 0);
	bool is_blocked() const;
	bool can_add_resource() const;
	bool all_queues_empty() const;
	bool request_exported_entity(uint32_t id, const ee_resource_callback_t& callback, bool now = true);

	std::deque<void*>* get_pending_queue() const;
	std::deque<void*>* get_loading_queue() const;
};

inline Singleton<ResourceStreamer, jc::resource_streamer::SINGLETON> g_rsrc_streamer;