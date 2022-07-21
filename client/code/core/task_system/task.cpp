#include <defs/standard.h>

#include <core/thread_system/thread_system.h>

#include "task.h"

Task::Task(const task_dispatch_t& dispatch, const task_completion_t& completion, bool async) : dispatch(dispatch), completion(completion), async(async)
{
	::check(dispatch && completion, "Invalid callbacks");
	::check(!async, "Async not supported yet");
	
	//f = std::async(async ? std::launch::async : std::launch::deferred, dispatch, this);
}

Task::~Task()
{
	//::check(!f.valid(), "The future of a task must be invalid when destroying it");
}

bool Task::check()
{
	if (state == TaskState_Dispatching && dispatch(this))
		state = TaskState_Completing;
	
	// if we reached here it means the task is async and we want to check its completion
	// by calling the completion callback until it returns true

	if (state == TaskState_Completing && completion(this))
	{
		state = TaskState_Completed;
		return false;
	}

	return true;
}