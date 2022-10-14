#pragma once

namespace jc::archives
{
	static constexpr uint32_t SINGLETON			= 0xAF236C;		// Archives*
	static constexpr uint32_t BORDERS			= 0x20;			// std::vector<size_t>
	static constexpr uint32_t UNKNOWN			= 0x30;			// size_t
	static constexpr uint32_t ASSETS_ENTRIES	= 0x34;			// std::vector<ArchiveAssetEntry>

	namespace fn
	{
		static constexpr uint32_t FIND_ASSET_ENTRY_BY_HASH	= 0x40B2F0;
		static constexpr uint32_t FIND_ASSET_ENTRY			= 0x40B4B0;
		static constexpr uint32_t FIND_ASSET				= 0x40B220;
		static constexpr uint32_t GET_ASSET_TYPE_BY_NAME	= 0x5C35D0;
	}
}

enum AssetType
{
	
};

struct AssetInfo
{
	std::string name;

	size_t data_base = 0u,
		   offset = 0u,
		   size = 0u;
};

struct ArchiveAssetEntry
{
	uint32_t hash = 0u;

	size_t offset = 0u,
		   size = 0u;
};

struct AssetDataInfo
{
	uint32_t hash = 0u;

	int32_t arc_index = -1;

	size_t offset = 0u,
		   size = 0u;
};

struct Asset
{
	uint8_t* data = nullptr;

	size_t size = 0u;

	Asset(size_t size) : size(size)
	{
		data = new uint8_t[size]();
	}

	~Asset()
	{
		delete[] data;
	}
};

struct AssetBuffer
{
	std::vector<uint8_t> data;

	size_t offset = 0u;

	template <typename T>
	T read()
	{
		const auto value = *BITCAST(T*, data.data() + offset);

		offset += sizeof(T);

		return value;
	}

	std::string read_str()
	{
		const auto len = read<size_t>();

		std::string out;

		out.resize(len);

		memcpy(out.data(), data.data() + offset, len);

		offset += len;

		return out;
	}

	void skip(size_t bytes)
	{
		offset += bytes;
	}
};

class Archive
{
private:

	std::string filename;

	HANDLE file = nullptr;

	int index = -1;

	size_t read_offset = 0u;

	void read_internal(void* data, size_t size);

public:

	Archive(const std::string& filename, int index);
	~Archive();

	void parse_sarcs();

	// file reading methods

	void seek(int32_t pos);
	void skip(int32_t bytes);

	int32_t get_curr_pointer() const;

	template <typename T>
	T read()
	{
		T value;
		read_internal(&value, sizeof(value));
		return value;
	}

	void read(size_t offset, void* data, size_t size)
	{
		seek(offset);
		read_internal(data, size);
	}

	std::string read_str();

	// asset methods

	std::shared_ptr<Asset> get_named_asset(const std::string& name, AssetInfo* info);
};

class Archives
{
public:

	void init();
	void destroy();
	void dump_hashed_assets();

	ArchiveAssetEntry* get_asset_entry(const std::string& name);
	ArchiveAssetEntry* get_asset_entry(uint32_t hash);

	AssetDataInfo get_asset_data_info(const ArchiveAssetEntry* entry) const;

	AssetDataInfo get_asset(const std::string& name);
	AssetDataInfo get_asset(uint32_t hash);

	std::vector<uint8_t> get_asset_data(const std::string& name);

	size_t get_unknown() const;

	int get_asset_type(const std::string& name) const;

	jc::stl::vector<size_t> get_borders() const;
	jc::stl::vector<ArchiveAssetEntry> get_assets_entries() const;
};

inline Singleton<Archives, jc::archives::SINGLETON> g_archives;