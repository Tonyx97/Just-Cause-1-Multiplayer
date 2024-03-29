#include <defs/standard.h>

#include "texture_system.h"

#include <game/object/asset/asset_texture.h>

namespace jc::texture_system
{
	std::unordered_map<std::string, shared_ptr<AssetTexture>> textures;
}

using namespace jc::texture_system;

void TextureSystem::init()
{
}

void TextureSystem::destroy()
{
}

bool TextureSystem::load_texture(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename);

	if (file_data.empty())
		return false;

	std::string clean_name = util::fs::strip_parent_path(filename);

	jc::stl::string name = clean_name;

	shared_ptr<AssetTexture> r;

	jc::v_call(this, vt::LOAD_TEXTURE_FROM_MEM, &r, name, file_data.data(), file_data.size());

	textures.insert({ clean_name, r });

	return true;
}

bool TextureSystem::unload_texture(const std::string& filename)
{
	auto it = textures.find(filename);
	if (it == textures.end())
		return false;

	textures.erase(it);

	return true;
}