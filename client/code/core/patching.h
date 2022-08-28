#pragma once

/*
* NOTE: Make sure the memory that we are going to patch has RW access
* or it will blow up
*/
template <bool scoped = true>
struct scoped_patch
{
	std::vector<uint8_t> previous {};

	void* address = nullptr;

	bool active = false;

	template <typename T>
	scoped_patch(T addy, const std::vector<uint8_t>& new_bytes = {})
		: address(BITCAST(void*, addy))
	{
		if (scoped && !new_bytes.empty())
			_do(new_bytes);
	}

	~scoped_patch()
	{
		if (scoped)
			_undo();
	}

	void _do(const std::vector<uint8_t>& new_bytes)
	{
		if (active)
			return;

		const auto size = new_bytes.size();

		previous.resize(size);

		memcpy(previous.data(), address, size);
		memcpy(address, new_bytes.data(), size);

		active = true;
	}

	void nop(int size)
	{
		std::vector<uint8_t> nop_bytes(size);

		std::ranges::generate_n(nop_bytes.begin(), nop_bytes.size(), []() { return 0x90; });

		_do(nop_bytes);
	}

	void _undo()
	{
		if (!active)
			return;

		active = false;

		memcpy(address, previous.data(), previous.size());
	}
};

struct patch : public scoped_patch<false>
{
};