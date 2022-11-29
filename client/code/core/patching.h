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

	scoped_patch() {}

	template <typename T>
	scoped_patch(T addy, const std::vector<uint8_t>& new_bytes = {})
		: address(BITCAST(void*, addy))
	{
		if (scoped && !new_bytes.empty())
			_do(new_bytes);
	}

	~scoped_patch()
	{
		if (scoped && active)
			_undo();
	}

	template <typename T>
	void set_address(const T& addy)
	{
		address = BITCAST(void*, addy);
	}

	void _do(const std::vector<uint8_t>& new_bytes)
	{
		check(address, "Invalid address");

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
		check(address, "Invalid address");

		std::vector<uint8_t> nop_bytes(size);

		std::ranges::generate_n(nop_bytes.begin(), nop_bytes.size(), []() { return 0x90; });

		_do(nop_bytes);
	}

	template <typename Tx, typename Ty>
	void jump(Tx from, Ty to)
	{
		address = BITCAST(void*, from);

		const auto offset = jc::calc_call_offset(address, to);

		_do(
		{
			0xE9,
			offset.b0,
			offset.b1,
			offset.b2,
			offset.b3,
		});
	}

	template <typename T>
	void jump(const T& to)
	{
		check(address, "Invalid address");

		jump(address, to);
	}

	void _undo()
	{
		if (!active)
			return;

		check(address, "Invalid address");

		active = false;

		memcpy(address, previous.data(), previous.size());
	}
};

struct patch : public scoped_patch<false>
{
};