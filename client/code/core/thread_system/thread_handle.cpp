#include <Windows.h>

#include <defs/standard.h>

#include "thread_handle.h"

Thread::Thread(const void* fn) : fn((void*)fn)
{
	handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)fn, this, 0, (DWORD*)&id);

	check(handle && handle != INVALID_HANDLE_VALUE, "Could not create engine thread");
}

Thread::~Thread()
{
	check(stopped, "Not allowed to terminate a thread that isn't stopped");
	check(CloseHandle(handle), "Thread handle could not be closed {:#x}", GetLastError());
}

void Thread::stop()
{
	cs.cancel();
}

void Thread::wait()
{
	check(WaitForSingleObject(handle, MAX_WAIT_TIMEOUT()) != WAIT_TIMEOUT, "A thread timed out because it's still running");

	stopped = true;
}

void Thread::suspend()
{
	SuspendThread(handle);
}

void Thread::resume()
{
	ResumeThread(handle);
}

void Thread::exit(uint32_t code)
{
	ExitThread(code);
}

void Thread::set_priority(int priority)
{
	check(SetThreadPriority(handle, priority), "Could not change thread priority");
}