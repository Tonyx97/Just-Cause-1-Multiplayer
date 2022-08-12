#pragma once

struct Buffer
{
	std::vector<uint8_t> data;

	Buffer() {}

	template <typename T>
	void insert(const T& v, size_t size)
	{
		const auto ptr = BITCAST(uint8_t*, &v);

		data.insert(data.end(), ptr, ptr + size);
	}

	template <typename T>
	void insert(T* v, size_t size)
	{
		data.insert(data.end(), v, v + size);
	}

	template <typename T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_array_v<T>>* = nullptr>
	void add(const T& v)
	{
		insert(v, sizeof(v));
	}

	void add(const std::string& v)
	{
		insert(v.length(), sizeof(size_t));
		insert(v.c_str(), v.length());
	}

	void to_file(const std::string& filename)
	{
		std::ofstream test_file(filename, std::ios::binary);

		test_file.write(BITCAST(char*, data.data()), data.size());
	}
};