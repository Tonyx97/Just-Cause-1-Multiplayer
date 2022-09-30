#pragma once

namespace jc::resource_streamer
{
	static constexpr uint32_t SINGLETON = 0xDA1C24; // ResourceStreamer*

	static constexpr uint32_t PENDING_QUEUE = 0x78; // std::deque<>
	static constexpr uint32_t LOADING_QUEUE	= 0x8C; // std::deque<>

	namespace fn
	{
		static constexpr uint32_t DISPATCH					= 0x989640;
		static constexpr uint32_t IS_BLOCKED				= 0x987FF0;
		static constexpr uint32_t CAN_ADD_RESOURCE			= 0x988010;
		static constexpr uint32_t ASSET_HOLDER_CTOR			= 0x463C20;
		
	}
}

class GameResource;
class ExportedEntityResource;
class AssetRBM;
class AssetPFX;
class AssetAnim;
class AssetTexture;

using ee_resource_callback_t = std::function<void(ExportedEntityResource*, const std::string& name)>;	// should replace this by agent type maybe

struct AssetDataHolder
{
	int unk[12];

	AssetDataHolder(const std::vector<uint8_t>& data)
	{
		jc::this_call(jc::resource_streamer::fn::ASSET_HOLDER_CTOR, this, data.data(), data.size());
	}
};

class ResourceStreamer
{
public:

	void init();
	void destroy();
	
	bool dispatch(uint32_t ms = 0);
	bool is_blocked() const;
	bool can_add_resource() const;
	bool all_queues_empty() const;
	bool request_agent_ee(int32_t id, const ee_resource_callback_t& callback, bool now = false);
	bool request_vehicle_ee(int32_t id, const ee_resource_callback_t& callback, bool now = false);

	std::deque<void*>* get_pending_queue() const;
	std::deque<void*>* get_loading_queue() const;
};

inline Singleton<ResourceStreamer, jc::resource_streamer::SINGLETON> g_rsrc_streamer;