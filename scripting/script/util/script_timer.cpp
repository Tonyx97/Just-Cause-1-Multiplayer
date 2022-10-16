#include <defs/standard.h>

#include <luas.h>

#include "script_timer.h"

ScriptTimer::ScriptTimer(luas::lua_fn& fn, std::vector<std::any>& args, int interval, int times) :
	fn(std::move(fn)),
	args(std::move(args)),
	interval(interval),
	times(times),
	times_remaining(times)
{
	last = std::chrono::steady_clock::now();
}

bool ScriptTimer::update()
{
	auto curr = std::chrono::steady_clock::now();

	if ((interval_left = int(std::chrono::duration_cast<std::chrono::milliseconds>(curr - last).count())) >= interval)
	{
		last = curr;

		--times_remaining;

		fn.call(args);
	}

	return (times_remaining == 0);
}

void ScriptTimer::reset()
{
	last = std::chrono::steady_clock::now();
	times_remaining = times;
	interval_left = interval;
}