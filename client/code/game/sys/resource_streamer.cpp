#include <defs/standard.h>

#include "resource_streamer.h"

#include <game/object/resource/ee_resource.h>

#include <core/task_system/task_system.h>

std::unordered_map<std::string, ref<AssetRBM>> rbms;
std::unordered_map<std::string, bref<AssetPFX>> pfxs;
std::unordered_map<std::string, ref<AssetAnim>> anims;
std::unordered_map<std::string, ref<AssetTexture>> textures;

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

bool ResourceStreamer::request_exported_entity(int32_t id, const ee_resource_callback_t& callback, bool now)
{
	auto it = jc::vars::exported_entities.find(id);
	if (it == jc::vars::exported_entities.end())
		return false;

	jc::stl::string ee_name = it->second;

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

bool ResourceStreamer::load_rbm(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename);

	if (file_data.empty())
		return false;

	jc::stl::string name = util::fs::strip_parent_path(filename);

	auto model = jc::this_call(jc::resource_streamer::fn::LOAD_RBM_FROM_MEM, jc::read<ptr>(0xD84F50), &name, file_data.data(), file_data.size(), 2);

	ref<AssetRBM> r;

	jc::this_call(jc::resource_streamer::fn::GET_RBM_REF, model, &r);

	rbms.insert({ filename, std::move(r) });

	jc::this_call(0x657B70, model);
	jc::c_call(0x405610, model);
	
    return true;
}

bool ResourceStreamer::load_pfx(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename);

	if (file_data.empty())
		return false;

	AssetDataHolder data_holder(file_data);

	jc::stl::string name = util::fs::strip_parent_path(filename);

	bref<AssetPFX> r(true);

	mat4 identity = mat4(1.f);

	jc::this_call(jc::resource_streamer::fn::LOAD_PFX_FROM_MEM, jc::read<ptr>(0xD37340), &name, &r, &identity, true, false, &data_holder);

	pfxs.insert({ filename, std::move(r) });

	return true;
}

bool ResourceStreamer::load_anim(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename);

	if (file_data.empty())
		return false;

	jc::stl::string name = util::fs::strip_parent_path(filename);

	ref<AssetAnim> r;

	jc::this_call(jc::resource_streamer::fn::LOAD_ANIM_FROM_MEM, jc::read<ptr>(0xD84D14), &r, &name, file_data.data(), file_data.size());

	anims.insert({ filename, std::move(r) });

	return true;
}

bool ResourceStreamer::load_texture(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename.c_str());

	if (file_data.empty())
		return false;
	
	jc::stl::string name = util::fs::strip_parent_path(filename);
	
	ref<AssetTexture> r;

	jc::this_call(jc::resource_streamer::fn::LOAD_TEXTURE_FROM_MEM, jc::read<ptr>(0xAF2410), &r, name, file_data.data(), file_data.size());

	textures.insert({ filename, std::move(r) });

	return true;
}

bool ResourceStreamer::unload_rbm(const std::string& filename)
{
	auto it = rbms.find(filename);
	if (it == rbms.end())
		return false;

	rbms.erase(it);

	return true;
}

bool ResourceStreamer::unload_pfx(const std::string& filename)
{
	auto it = pfxs.find(filename);
	if (it == pfxs.end())
		return false;

	pfxs.erase(it);

	return true;
}

bool ResourceStreamer::unload_anim(const std::string& filename)
{
	auto it = anims.find(filename);
	if (it == anims.end())
		return false;

	anims.erase(it);

	return true;
}

bool ResourceStreamer::unload_texture(const std::string& filename)
{
	auto it = textures.find(filename);
	if (it == textures.end())
		return false;

	textures.erase(it);

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