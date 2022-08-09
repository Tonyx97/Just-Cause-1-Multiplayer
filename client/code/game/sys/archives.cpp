#include <defs/standard.h>

#include "archives.h"

#include <game/object/vars/archives.h>

#define DUMP_SARCS 0
#define EXTRACT_FILES 0

namespace jc::archives
{
#if DUMP_SARCS
	std::mutex mtx;
	std::set<std::string> files_dump;
#endif

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

	read_metadata();
}

Archive::~Archive()
{
	CloseHandle(file);
}

void Archive::read_metadata()
{
	using namespace jc::archives;

#if DUMP_SARCS
	if (index == 0)
		seek(0x1AC10000); // for some reason, this is where subarchives begin

	std::ofstream out_file(filename + ".dmp");

	out_file << FORMATV("uint32_t pc{}_sarcs[] = {{", index) << '\n';

	while (get_curr_pointer() <= 0x40000000)
	{
		const auto header = read<uint64_t>();

		if (header == 0x4352415300000004)
			out_file << FORMATV("\t0x{:x},", get_curr_pointer() - 0x8) << '\n';

		if (GetAsyncKeyState(VK_F5))
		{
			log(RED, "{} -> 0x{:x}", filename, get_curr_pointer());
			Sleep(200);
		}
	}

	out_file << "}};";

	out_file.close();

	log(GREEN, "{} dumped", filename);
#endif

	uint32_t* sarcs = nullptr;

	size_t sarcs_count = 0u;

	switch (index)
	{
	case 0: sarcs = jc::vars::pc0_sarcs; sarcs_count = sizeof(jc::vars::pc0_sarcs) / sizeof(uint32_t); break;
	case 1: sarcs = jc::vars::pc1_sarcs; sarcs_count = sizeof(jc::vars::pc1_sarcs) / sizeof(uint32_t); break;
	case 2: sarcs = jc::vars::pc2_sarcs; sarcs_count = sizeof(jc::vars::pc2_sarcs) / sizeof(uint32_t); break;
	case 3: sarcs = jc::vars::pc3_sarcs; sarcs_count = sizeof(jc::vars::pc3_sarcs) / sizeof(uint32_t); break;
	case 4: sarcs = jc::vars::pc4_sarcs; sarcs_count = sizeof(jc::vars::pc4_sarcs) / sizeof(uint32_t); break;
	}

	for (size_t i = 0u; i < sarcs_count; ++i)
	{
		// move to the current sarc

		const auto sarc_base = static_cast<size_t>(sarcs[i]);

		// skip the header trash

		seek(sarc_base + 0xC);

		// get metadata size

		const auto metadata_end = sarc_base + read<size_t>();

		while (static_cast<size_t>(get_curr_pointer()) < metadata_end)
		{
			const auto rsrc_name = read_str();
			const auto rsrc_offset = read<size_t>();
			const auto rsrc_size = read<size_t>();

			metadata.insert({ rsrc_name,
			{
				.name = rsrc_name,
				.data_base = sarc_base,
				.offset = rsrc_offset,
				.size = rsrc_size,
			} });

			/*static bool trash = false;
			if (index == 3 && !trash)
			{
				auto curr_ptr = get_curr_pointer();

				auto anim_pos = 0x1DFAD800;
				auto ay_size = 0x1DFAD804;
				char* asset_data = new char[ay_size]();

				read_internal(asset_data, ay_size);

				for (int i = 0; i < anim_pos + 0x4; ++i)
				{
					auto off = *(uint32_t*)(asset_data + i);

					if (i + off >= anim_pos - 0x10 && i + off <= anim_pos + 0x10)
						log(RED, "FOUND? {:x}", i);

					if (GetAsyncKeyState(VK_F5))
					{
						log(RED, "{:x}", i);
						Sleep(200);
					}
				}

				delete[] asset_data;

				seek(curr_ptr);

				trash = true;
			}*/

#if EXTRACT_FILES
			std::string filename = rsrc_name;

			if (auto i = filename.find_last_of('\\'); i != -1)
				filename = filename.substr(i + 1);

			auto curr_ptr = get_curr_pointer();

			char* asset_data = new char[rsrc_size]();

			seek(sarc_base + rsrc_offset);
			read_internal(asset_data, rsrc_size);

			std::ofstream asset_file("ExtractedAssets\\named\\" + filename, std::ios::binary);
			asset_file.write(asset_data, rsrc_size);
			asset_file.close();

			delete[] asset_data;

			seek(curr_ptr);
#endif

#if DUMP_SARCS
			std::lock_guard lock(mtx);

			files_dump.insert(rsrc_name);
#endif
		}
	}
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

#if DUMP_SARCS
	std::ofstream assets_dump("assets_list.txt");

	for (const auto& file : files_dump)
		assets_dump << file << '\n';

	assets_dump.close();
#endif
}

void Archives::destroy()
{
	using namespace jc::archives;

	for (auto arc : arcs)
		JC_FREE(arc);
}

void Archives::dump_asset(const std::string& name)
{
	using namespace jc::archives;

	for (auto arc : arcs)
	{
		AssetInfo info;

		if (const auto asset = arc->get_named_asset(name, &info))
		{
			std::string filename = name;

			if (auto i = name.find_last_of('\\'); i != -1)
				filename = name.substr(i + 1);

			std::ofstream asset_file("ExtractedAssets\\" + filename, std::ios::binary);

			asset_file.write((char*)asset->data, asset->size);

			return;
		}
	}
}

void Archives::dump_hashed_assets()
{
	const auto entries = get_assets_entries();

	for (const auto& entry : entries)
	{
		const auto data_info = get_asset_data_info(&entry);

		log(RED, "[pc{}.arc] {:x} {:x} {:x}", data_info.arc_index, data_info.hash, data_info.offset, data_info.size);

		const auto arc = jc::archives::arcs[data_info.arc_index];

		char* data = new char[data_info.size]();

		arc->read(data_info.offset, data, data_info.size);

		std::string format;

		switch (jc::read<uint32_t>(data))
		{
		case 0x4: log(CYAN, "Asset is .ee"); format = ".ee"; break;
		case 0x5: log(CYAN, "Asset is .rbm"); format = ".rbm"; break;
		case 0x4D494E41: log(CYAN, "Asset is .anim"); format = ".anim"; break;
		case 0x20534444: log(CYAN, "Asset is .dds"); format = ".dds"; break;
		case 0x584650: log(CYAN, "Asset is .pfx"); format = ".pfx"; break;
		/*default:
			log(BLUE, "'{}'", (char*)data);*/
		}

		if (!format.empty())
		{
			std::stringstream ss;

			ss << std::hex << data_info.hash;

			std::ofstream asset_file("ExtractedAssets\\hashed\\" + ss.str() + format, std::ios::binary);

			asset_file.write(data, data_info.size);
			asset_file.close();
		}

		delete[] data;
	}
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

jc::stl::vector<size_t> Archives::get_borders() const
{
	return jc::read<jc::stl::vector<size_t>>(this, jc::archives::BORDERS);
}

jc::stl::vector<ArchiveAssetEntry> Archives::get_assets_entries() const
{
	return jc::read<jc::stl::vector<ArchiveAssetEntry>>(this, jc::archives::ASSETS_ENTRIES);
}