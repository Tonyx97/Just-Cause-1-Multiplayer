#pragma once

#include <fstream>

struct Buffer
{
	std::vector<uint8_t> data;

	Buffer() {}

	void clear() { data.clear(); }

	template <typename T>
	void insert(const T& v, size_t size) requires (!std::is_pointer_v<T>)
	{
		const auto ptr = std::bit_cast<uint8_t*>(&v);

		data.insert(data.end(), ptr, ptr + size);
	}

	template <typename T>
	void insert(T* v, size_t size)
	{
		const auto v_ptr = std::bit_cast<uint8_t*>(v);

		data.insert(data.end(), v_ptr, v_ptr + size);
	}

	template <typename T, std::enable_if_t<!std::is_pointer_v<T> && !std::is_array_v<T>>* = nullptr>
	void add(const T& v)
	{
		insert(v, sizeof(v));
	}

	void add(const std::string& v)
	{
		insert(v.length(), sizeof(size_t));
		insert(std::bit_cast<uint8_t*>(v.data()), v.length());
	}

	template <typename T, typename... A>
	void add(const T& v, A... args)
	{
		if constexpr (std::is_array_v<T>)
			add(std::string(v));
		else add(v);

		if constexpr (sizeof...(args) > 0)
			add(args...);
	}

	template <typename T>
	T get(size_t off = 0u) const
	{
		if (off >= data.size())
			return {};

		if constexpr (std::is_same_v<T, std::string>)
		{
			const auto len = *std::bit_cast<size_t*>(data.data() + std::exchange(off, off + sizeof(size_t)));

			if (off >= data.size())
				return {};

			std::string out;

			out.resize(len);

			memcpy(out.data(), data.data() + off, len);

			return out;
		}
		else return *std::bit_cast<T*>(data.data() + off);
	}

	void to_file(const std::string& filename)
	{
		std::ofstream test_file(filename, std::ios::binary);

		test_file.write(std::bit_cast<char*>(data.data()), data.size());
	}

	void copy_to(uint8_t* dst)
	{
		memcpy(dst, data.data(), data.size());
	}
};