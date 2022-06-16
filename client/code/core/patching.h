#pragma once

/*
* NOTE: Make sure the memory that we are going to patch has RW access
* or it will blow up
*/
template <size_t S>
struct scoped_patch
{
	uint8_t previous[S] = { 0u };

	void* address = nullptr;

	template <typename T>
	constexpr scoped_patch(T addy, const std::vector<uint8_t>& new_bytes)
		: address(BITCAST(void*, addy))
	{
		memcpy(previous, address, S);
		memcpy(address, new_bytes.data(), S);
	}

	constexpr ~scoped_patch()
	{
		memcpy(address, previous, S);
	}
};