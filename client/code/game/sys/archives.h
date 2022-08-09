#pragma once

namespace jc::archives
{
	static constexpr uint32_t SINGLETON		= 0xAF236C;			// Archives*
}

struct AssetInfo
{
	std::string name;

	size_t data_base = 0u,
		   offset = 0u,
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

class Archive
{
private:

	std::vector<Archive*> subarchives;

	std::string filename;

	HANDLE file = nullptr;

	int index = -1;

	std::unordered_map<std::string, AssetInfo> metadata;

	void read_metadata();
	void read_internal(void* data, size_t size);

public:

	Archive(const std::string& filename, int index);
	~Archive();

	// file reading methods

	void seek(int32_t pos);

	int32_t get_curr_pointer() const;

	template <typename T>
	T read()
	{
		T value;
		read_internal(&value, sizeof(value));
		return value;
	}

	std::string read_str();

	// asset methods

	std::shared_ptr<Asset> get_asset(const std::string& name, AssetInfo* info);
};

class Archives
{
public:

	void init();
	void destroy();
	void dump_asset(const std::string& name);
};

inline Singleton<Archives, jc::archives::SINGLETON> g_archives;