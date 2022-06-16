#include <defs/standard.h>

#include "timer.h"

namespace timer
{
	std::vector<Timer*> timers;
}

Timer* timer::add_timer(int64_t interval, const timer_fn_t& fn)
{
	auto timer = JC_ALLOC(Timer, interval, fn);
	
	timers.push_back(timer);

	return timer;
}

void timer::dispatch()
{
	for (auto timer : timers)
		timer->update();
}

void timer::remove_timer(Timer* timer)
{
	JC_FREE(timer);
}

void timer::clear_timers()
{
	for (auto timer : timers)
		remove_timer(timer);

	timers.clear();
}