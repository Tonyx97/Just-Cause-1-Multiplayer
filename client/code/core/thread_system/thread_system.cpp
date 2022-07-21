#ifdef _WIN32
#include <Windows.h>
#endif

#include <defs/standard.h>

#include "thread_system.h"
#include "thread_handle.h"

void ThreadSystem::stop_threads_internal(Thread** list, size_t count)
{
	for (size_t i = 0ull; i < count; ++i)
		list[i]->stop();

	for (size_t i = 0ull; i < count; ++i)
	{
		auto thread = list[i];

		thread->wait();

		remove_thread(thread);
	}
}

void ThreadSystem::remove_thread(Thread* thread)
{
	if (auto node = threads.extract(thread); !node.empty())
		JC_FREE(node.value());
	else check(false, "Trying to stop an invalid thread");
}

ThreadSystem::ThreadSystem()
{
	main_thread_id = GetCurrentThreadId();
}

void ThreadSystem::pause()
{
#ifdef _WIN32
	SwitchToThread();
#else
	_mm_pause();
#endif
}

void ThreadSystem::suspend_all()
{
	for (auto thread : threads)
		thread->suspend();
}

void ThreadSystem::resume_all()
{
	for (auto thread : threads)
		thread->resume();
}

void ThreadSystem::check_main_thread()
{
	check(GetCurrentThreadId() == main_thread_id, "Main thread check failed");
}

Thread* ThreadSystem::start_thread(const void* fn)
{
	check(GetCurrentThreadId() == main_thread_id, "Thread creation only allowed from the main thread");
	check(threads.size() < MAX_THREADS(), "Exceeded the amount of engine threads running");

	auto thread = JC_ALLOC(Thread, fn);

	threads.insert(thread);

	return thread;
}