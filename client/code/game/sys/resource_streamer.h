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
		static constexpr uint32_t LOAD_RBM_FROM_MEM			= 0x57A070;
		static constexpr uint32_t GET_RBM_REF				= 0x5C3570;
		static constexpr uint32_t LOAD_PFX_FROM_MEM			= 0x4E4760;
		static constexpr uint32_t LOAD_ANIM_FROM_MEM		= 0x55F170;
		static constexpr uint32_t LOAD_TEXTURE_FROM_MEM		= 0x423070;
		static constexpr uint32_t ASSET_HOLDER_CTOR			= 0x463C20;
		
	}
}

class Resource;
class ExportedEntityResource;
class AssetRBM;
class AssetPFX;
class AssetAnim;
class AssetTexture;

using resource_callback_t = std::function<void(Resource*)>;
using ee_resource_callback_t = std::function<void(ExportedEntityResource*)>;

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
	bool request_exported_entity(int32_t id, const ee_resource_callback_t& callback, bool now = false);
	
	bool load_rbm(const std::string& filename);
	bool load_pfx(const std::string& filename);
	bool load_anim(const std::string& filename);
	bool load_texture(const std::string& filename);

	bool unload_rbm(const std::string& filename);
	bool unload_pfx(const std::string& filename);
	bool unload_anim(const std::string& filename);
	bool unload_texture(const std::string& filename);

	std::deque<void*>* get_pending_queue() const;
	std::deque<void*>* get_loading_queue() const;
};

inline Singleton<ResourceStreamer, jc::resource_streamer::SINGLETON> g_rsrc_streamer;