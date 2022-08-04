#pragma once

namespace jc::savegame
{
	static constexpr uint32_t HP_SCALE			= 0x0;
	static constexpr uint32_t LOCAL_TRANSFORM	= 0x4;
}

struct SaveGameBuffer
{
	std::vector<uint8_t> data;

	template <typename T>
	void add(const T& value)
	{
		const auto value_ptr = BITCAST(uint8_t*, &value);

		data.insert(data.end(), value_ptr, value_ptr + sizeof(T));
	}

	void copy_to(uint8_t* dst)
	{
		memcpy(dst, data.data(), data.size());
	}
};