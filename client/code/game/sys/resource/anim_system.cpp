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

	jc::stl::string name = util::fs::strip_parent_path(filename);

	shared_ptr<AssetAnim> r;

	jc::this_call(fn::LOAD_ANIM_FROM_MEM, this, &r, &name, file_data.data(), file_data.size());

	anims.insert({ filename, r });

	return true;
}

bool AnimSystem::unload_anim(const std::string& filename)
{
	auto it = anims.find(filename);
	if (it == anims.end())
		return false;

	anims.erase(it);

	return true;
}