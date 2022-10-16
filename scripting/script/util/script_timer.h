#pragma once

class ScriptTimer
{
private:

	luas::lua_fn fn;

	std::vector<std::any> args;

	std::chrono::steady_clock::time_point last;

	int interval = 0,
		interval_left = 0,
		times = 0,
		times_remaining = 0;

	bool pending_kill = false;

public:

#ifdef JC_CLIENT
	static constexpr auto INTERVAL_MIN() { return 50; }
#else
	static constexpr auto INTERVAL_MIN() { return 25; }
#endif

	ScriptTimer(luas::lua_fn& fn, std::vector<std::any>& args, int interval, int times);

	void reset();
	void set_pending_kill() { pending_kill = true; }

	bool update();
	bool is_pending_kill() const { return pending_kill; }

	int get_interval() const { return interval; }
	int get_interval_left() const { return interval_left; }
	int get_times() const { return times; }
	int get_times_remaining() const { return times_remaining; }
};