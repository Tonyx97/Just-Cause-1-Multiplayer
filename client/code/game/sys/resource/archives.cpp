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

	std::mutex metadata_mtx;

	std::unordered_map<std::string, AssetInfo> metadata;
}

using namespace jc::archives;

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

void Archive::skip(int32_t bytes)
{
	SetFilePointer(file, bytes, nullptr, FILE_CURRENT);
}

uint32_t Archive::get_curr_pointer() const
{
	return SetFilePointer(file, 0, nullptr, FILE_CURRENT);
}

std::string Archive::read_str()
{
	const auto len = read<int>();

	std::string str;

	str.resize(len);

	read_internal(str.data(), len);
	
	return str;
}

void Archives::init()
{
	const auto base_path = std::string(ARCHIVES_PATH);

	for (int i = 0; i < MAX_ARCHIVES; ++i)
	{
		const auto filename = base_path + NAME + std::to_string(i) + FORMAT;

		arcs[i] = JC_ALLOC(Archive, filename, i);
	}

	parse_sarcs();
}

void Archives::destroy()
{
	for (auto arc : arcs)
		JC_FREE(arc);
}

void Archives::parse_sarcs()
{
	log(YELLOW, "Parsing .arc files...");

	TimeProfiling p("Time parsing .arc files");

	const auto entries = g_archives->get_assets_entries();

	std::vector<std::vector<AssetDataInfo>> arcs_assets_list(MAX_ARCHIVES);

	// associate asset info lists with their arc
	
	for (const auto& entry : entries)
		if (auto data_info = g_archives->get_asset_data_info(&entry); data_info.arc_index >= 0 && data_info.arc_index < MAX_ARCHIVES)
			arcs_assets_list[data_info.arc_index].push_back(std::move(data_info));

	// launch an async task for each arc to get the assets metadata

	std::vector<std::future<void>> futures;

	for (const auto& asset_list : arcs_assets_list)
	{
		futures.push_back(std::async(std::launch::async, [](const std::vector<AssetDataInfo>& asset_list)
		{
			for (const auto& data_info : asset_list)
			{
				const auto arc = arcs[data_info.arc_index];

				arc->seek(data_info.offset);

				const auto base_offset = arc->get_curr_pointer();

				switch (arc->read<uint32_t>())
				{
				case 0x4: // .ee
				{
					arc->skip(sizeof(uint32_t) * 2);

					const auto data_begin_offset = arc->read<uint32_t>();
					const auto data_offset = base_offset + data_begin_offset;

					while (arc->get_curr_pointer() < data_offset)
					{
						const auto file_name = arc->read_str();

						metadata_mtx.lock();

						if (!metadata.contains(file_name))
						{
							struct offset_and_size
							{
								uint32_t offset, size;
							} offset_n_size = arc->read<offset_and_size>();

							offset_n_size.offset += base_offset;

							metadata.insert({ file_name,
							{
								.name = file_name,
								.arc_index = data_info.arc_index,
								.offset = offset_n_size.offset,
								.size = offset_n_size.size,
							} });

							metadata_mtx.unlock();
						}
						else
						{
							metadata_mtx.unlock();

							arc->skip(sizeof(uint32_t) * 2);
						}
					}

					break;
				}
				case 0x4D494E41:
				{
					break;
				}
				}
			}
		}, asset_list));
	}

	// wait until all arcs are completely parsed and
	// metadata is filled
	
	for (auto& f : futures)
		f.wait();
}

ArchiveAssetEntry* Archives::get_asset_entry(const std::string& name)
{
	return jc::this_call<ArchiveAssetEntry*>(fn::FIND_ASSET_ENTRY, this, name.c_str());
}

ArchiveAssetEntry* Archives::get_asset_entry(uint32_t hash)
{
	const auto entries = get_assets_entries();

	ArchiveAssetEntry* entry = nullptr;

	jc::c_call(fn::FIND_ASSET_ENTRY_BY_HASH, &entry, entries.begin(), entries.end(), &hash);

	return entry;
}

AssetDataInfo Archives::get_asset_data_info(const ArchiveAssetEntry* entry) const
{
	AssetDataInfo info;

	info.hash = entry->hash;
	info.size = entry->size;

	jc::this_call(fn::FIND_ASSET, this, entry->offset, &info.arc_index, &info.offset);

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

std::vector<uint8_t> Archives::get_asset_data(const std::string& name)
{
	const auto it = metadata.find(name);

	AssetInfo info {};

	if (it == metadata.end())
	{
		// if it's not in the general metadata, try to get the data from the single files

		auto entry = get_asset(name);

		if (entry.arc_index == -1)
			return {};

		info.name = name;
		info.arc_index = entry.arc_index;
		info.offset = entry.offset;
		info.size = entry.size;
	}
	else info = it->second;

	if (info.arc_index < 0 || info.arc_index >= MAX_ARCHIVES)
		return {};

	const auto arc = arcs[info.arc_index];

	std::vector<uint8_t> out;

	out.resize(info.size);

	arc->read(info.offset, out.data(), out.size());

	return out;
}

size_t Archives::get_unknown() const
{
	return jc::read<size_t>(this, UNKNOWN);
}

int Archives::get_asset_type(const std::string& name) const
{
	const jc::stl::string str = name;

	return jc::std_call<int>(fn::GET_ASSET_TYPE_BY_NAME, &name);
}

jc::stl::vector<size_t> Archives::get_borders() const
{
	return jc::read<jc::stl::vector<size_t>>(this, BORDERS);
}

jc::stl::vector<ArchiveAssetEntry> Archives::get_assets_entries() const
{
	return jc::read<jc::stl::vector<ArchiveAssetEntry>>(this, ASSETS_ENTRIES);
}