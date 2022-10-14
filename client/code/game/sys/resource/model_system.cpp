#include <defs/standard.h>

#include "model_system.h"

#include <game/object/asset/asset_rbm.h>

namespace jc::model_system
{
	std::unordered_map<std::string, ref<AssetRBM>> rbms;
}

using namespace jc::model_system;

void ModelSystem::init()
{
}

void ModelSystem::destroy()
{
}

bool ModelSystem::load_rbm(const std::string& filename)
{
	const auto file_data = util::fs::read_bin_file(filename);

	if (file_data.empty())
		return false;

	return load_rbm(filename, file_data);
}

bool ModelSystem::load_rbm(const std::string& filename, const std::vector<uint8_t>& data)
{
	jc::stl::string name = util::fs::strip_parent_path(filename);

	auto model = jc::this_call<AssetRBM*>(fn::LOAD_RBM_FROM_MEM, this, &name, data.data(), data.size(), 2);
	auto r = model->get_ref();

	rbms.insert({ filename, std::move(r) });

	// i don't know about this but the engine does this lol

	jc::this_call(0x657B70, model);
	jc::c_call(0x405610, model);

	return true;
}

bool ModelSystem::unload_rbm(const std::string& filename)
{
	auto it = rbms.find(filename);
	if (it == rbms.end())
		return false;

	rbms.erase(it);

	return true;
}