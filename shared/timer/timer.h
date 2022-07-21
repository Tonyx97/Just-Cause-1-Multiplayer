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

class TimerRaw
{
private:

	std::chrono::steady_clock::time_point last, curr;
	
	int64_t interval = 0;

public:

	TimerRaw() { reset(); }
	TimerRaw(int64_t interval) { reset(interval); }

	void reset(int64_t val = 0)
	{
		last = curr = std::chrono::steady_clock::now();
		interval = val;
	}

	void operator () (int64_t interval)
	{
		last = curr = std::chrono::steady_clock::now();
		this->interval = interval;
	}

	int64_t get_interval() const { return interval; }

	bool ready()
	{
		if (interval <= 0)
			return false;

		curr = std::chrono::steady_clock::now();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(curr - last).count() >= interval)
		{
			last = curr;

			return true;
		}

		return false;
	}
};