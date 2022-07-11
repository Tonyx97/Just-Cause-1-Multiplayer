#pragma once

/*
* NOTE: Make sure the memory that we are going to patch has RW access
* or it will blow up
*/
template <size_t S, bool scoped = true>
struct scoped_patch
{
	uint8_t previous[S] = { 0u };

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

		memcpy(previous, address, S);
		memcpy(address, new_bytes.data(), S);

		active = true;
	}

	void _undo()
	{
		if (!active)
			return;

		active = false;

		memcpy(address, previous, S);
	}
};

template <size_t S>
struct patch : public scoped_patch<S, false>
{
};