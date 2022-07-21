#include <defs/standard.h>

#include "task_system.h"

void TaskSystem::init()
{
}

void TaskSystem::destroy()
{
	check(pending_tasks.empty(), "Running task list must be empty");
	check(queued_tasks.empty(), "Task queue must be empty");
}

void TaskSystem::process(bool wait)
{
	check(!processing, "Cannot process tasks while processing tasks");

	processing = true;

	// transfer queued tasks to running tasks

	pending_tasks.insert(pending_tasks.end(), queued_tasks.begin(), queued_tasks.end());

	queued_tasks.clear();

	// if "wait" is true it means we are going to wait until all queued and pending
	// tasks are fully completed, otherwise, will simply try to dispatch them

	do {
		util::container::vector::remove_and_free_if(pending_tasks, [&](Task* task)
		{
			return task->check();
		});
	} while (wait && has_pending_tasks());

	processing = false;
}

Task* TaskSystem::launch_sync_task(const task_dispatch_t& dispatch, const task_completion_t& completion)
{
	auto task = JC_ALLOC(Task, dispatch, completion, false);

	queued_tasks.push_back(task);

	return task;
}

Task* TaskSystem::launch_async_task(const task_dispatch_t& dispatch, const task_completion_t& completion)
{
	auto task = JC_ALLOC(Task, dispatch, completion, true);

	queued_tasks.push_back(task);

	return task;
}