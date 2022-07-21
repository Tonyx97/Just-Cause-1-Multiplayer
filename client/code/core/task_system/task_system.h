#pragma once

#include "task.h"

/**
* Class taken from my TombVG 3D engine to handle async tasks but in this case
* it's modified to handle sync tasks because there is no need for async tasks here
* NOTE: Make sure we check the instances in the callbacks because the game or the mod
* may destroy stuff while the task is being processed so the instances would turn invalid
*/
class TaskSystem
{
private:

	std::vector<Task*> pending_tasks,
					   queued_tasks;

	bool processing = false;

public:

	TaskSystem()														{}
	~TaskSystem()														{}

	void init();
	void destroy();

	/**
	* Processses all pending tasks
	* NOTE: Tasks must be processed first thing before doing any logic or rendering,
	* so basically at the very beginning of the frame
	* @param wait Specifies if we should wait until all tasks are completed
	*/
	void process(bool wait = false);

	bool has_pending_tasks() const	{ return !pending_tasks.empty(); }
	bool has_queued_tasks() const	{ return !queued_tasks.empty(); }
	bool can_be_destroyed() const	{ return !has_pending_tasks() && !has_queued_tasks(); }

	Task* launch_sync_task(const task_dispatch_t& dispatch, const task_completion_t& completion);
	Task* launch_async_task(const task_dispatch_t& dispatch, const task_completion_t& completion);
};

inline TaskSystem* g_task = nullptr;