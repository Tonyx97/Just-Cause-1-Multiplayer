#pragma once

using timer_fn_t = std::function<void()>;

class Timer;

namespace timer
{
	Timer* add_timer(int64_t interval, const timer_fn_t& fn);

	void dispatch();
	void remove_timer(Timer* timer);
	void clear_timers();
}

class Timer
{
private:
	std::chrono::steady_clock::time_point a;

	std::function<void()> fn;

	int64_t interval = 0ull;

public:
	Timer(int64_t interval, const timer_fn_t& fn)
		: interval(interval)
		, fn(fn)
	{
		a = std::chrono::steady_clock::now();
	}

	void update()
	{
		const auto b = std::chrono::steady_clock::now();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count() >= interval)
		{
			fn();

			a = b;
		}
	}
};