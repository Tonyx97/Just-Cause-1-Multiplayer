#include <defs/standard.h>

#include "archives.h"

#define EXTRACT_FILES 0

namespace jc::archives
{
	static constexpr auto MAX_ARCHIVES = 5;
	static constexpr auto ARCHIVES_PATH = "Archives\\";
	static constexpr auto NAME = "pc";
	static constexpr auto FORMAT = ".arc";

	Archive* arcs[MAX_ARCHIVES] = { nullptr };
}

Archive::Archive(const std::string& filename, int index) : filename(filename), index(index)
{
	file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

	check(file && file != INVALID_HANDLE_VALUE, "Could not open game archives");

	log(GREEN, "Game archive '{}': 0x{:x}", filename, ptr(file));
}

Archive::~Archive()
{
	CloseHandle(file);
}

void Archive::read_internal(void* data, size_t size)
{
	if (!ReadFile(file, data, size, nullptr, nullptr))
		log(RED, "[{}] Error reading '{}': 0x{:x}", CURR_FN, filename, GetLastError());
}

void Archive::seek(int32_t pos)
{
	SetFilePointer(file, pos, nullptr, FILE_BEGIN);
}

int32_t Archive::get_curr_pointer() const
{
	return static_cast<int32_t>(SetFilePointer(file, 0, nullptr, FILE_CURRENT));
}

std::string Archive::read_str()
{
	const auto len = read<int>();

	std::string str;

	str.resize(len);

	read_internal(str.data(), len);
	
	return str;
}

std::shared_ptr<Asset> Archive::get_named_asset(const std::string& name, AssetInfo* info)
{
	auto it = metadata.find(name);
	if (it == metadata.end())
		return {};

	const auto asset_info = it->second;
	const auto asset = JC_ALLOC(Asset, asset_info.size);

	*info = asset_info;

	seek(asset_info.data_base + asset_info.offset);
	read_internal(asset->data, asset->size);

	return std::shared_ptr<Asset>(asset);
}

void Archives::init()
{
	using namespace jc::archives;

	const auto base_path = std::string(ARCHIVES_PATH);

	std::vector<std::future<void>> futures;

	for (int i = 0; i < MAX_ARCHIVES; ++i)
	{
		futures.push_back(std::async(std::launch::async, [=]()
		{
			const auto filename = base_path + NAME + std::to_string(i) + FORMAT;

			arcs[i] = JC_ALLOC(Archive, filename, i);
		}));
	}

	for (auto& f : futures)
		f.wait();
}

void Archives::destroy()
{
	using namespace jc::archives;

	for (auto arc : arcs)
		JC_FREE(arc);
}

ArchiveAssetEntry* Archives::get_asset_entry(const std::string& name)
{
	return jc::this_call<ArchiveAssetEntry*>(jc::archives::fn::FIND_ASSET_ENTRY, this, name.c_str());
}

ArchiveAssetEntry* Archives::get_asset_entry(uint32_t hash)
{
	const auto entries = get_assets_entries();

	ArchiveAssetEntry* entry = nullptr;

	jc::c_call(jc::archives::fn::FIND_ASSET_ENTRY_BY_HASH, &entry, entries.begin(), entries.end(), &hash);

	return entry;
}

AssetDataInfo Archives::get_asset_data_info(const ArchiveAssetEntry* entry) const
{
	AssetDataInfo info;

	info.hash = entry->hash;
	info.size = entry->size;

	jc::this_call(jc::archives::fn::FIND_ASSET, this, entry->offset, &info.arc_index, &info.offset);

	return info;
}

AssetDataInfo Archives::get_asset(const std::string& name)
{
	if (const auto entry = get_asset_entry(name))
		return get_asset_data_info(entry);

	return {};
}

AssetDataInfo Archives::get_asset(uint32_t hash)
{
	if (const auto entry = get_asset_entry(hash))
		return get_asset_data_info(entry);

	return {};
}

size_t Archives::get_unknown() const
{
	return jc::read<size_t>(this, jc::archives::UNKNOWN);
}

int Archives::get_asset_type(const std::string& name) const
{
	const jc::stl::string str = name;

	return jc::std_call<int>(jc::archives::fn::GET_ASSET_TYPE_BY_NAME, &name);
}

jc::stl::vector<size_t> Archives::get_borders() const
{
	return jc::read<jc::stl::vector<size_t>>(this, jc::archives::BORDERS);
}

jc::stl::vector<ArchiveAssetEntry> Archives::get_assets_entries() const
{
	return jc::read<jc::stl::vector<ArchiveAssetEntry>>(this, jc::archives::ASSETS_ENTRIES);
}