#pragma once

using namespace std::chrono_literals;

#include "cancellable_sleep.h"

class Thread
{
private:

	CancellableSleep cs;

	void* handle = nullptr,
		* fn = nullptr;

	uint32_t id = 0;

	bool stopped = false;

public:

	Thread(const void* fn);
	~Thread();

	static constexpr uint32_t MAX_WAIT_TIMEOUT()	{ return 5 * 1000; }

	void suspend();
	void resume();
	void stop();
	void wait();
	void exit(uint32_t code = 0);
	void set_priority(int priority);

	template <typename T>
	bool sleep(T duration)							{ return cs.sleep(duration); }

	uint32_t get_id() const							{ return id; }

	void* get_handle() const						{ return handle; }
};