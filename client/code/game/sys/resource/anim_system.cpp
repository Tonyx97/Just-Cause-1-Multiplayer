#include <defs/standard.h>

#include "anim_system.h"

#include <game/object/asset/asset_anim.h>

namespace jc::anim_system
{
	std::unordered_map<std::string, shared_ptr<AssetAnim>> anims;
}

using namespace jc::anim_system;

void AnimSystem::init()
{
}

void AnimSystem::destroy()
{
}

bool AnimSystem::load_anim(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename);

	if (file_data.empty())
		return false;

	return load_anim(filename, file_data);
}

bool AnimSystem::load_anim(const std::string& filename, const std::vector<uint8_t>& data)
{
	std::string clean_name = util::fs::strip_parent_path(filename);

	jc::stl::string name = clean_name;

	shared_ptr<AssetAnim> r;

	jc::this_call(fn::LOAD_ANIM_FROM_MEM, this, &r, &name, data.data(), data.size());

	anims.insert({ clean_name, r });

	return true;
}

bool AnimSystem::load_anim_async(const std::string& filename)
{
	ptr shared_ptr_temp[2] = { 0 };

	const jc::stl::string _filename = filename;

	return !!jc::this_call<ptr*>(fn::LOAD_ANIM_ASYNC, this, shared_ptr_temp, &_filename);
}

bool AnimSystem::unload_anim(const std::string& filename)
{
	auto it = anims.find(filename);
	if (it == anims.end())
		return false;

	anims.erase(it);

	return true;
}