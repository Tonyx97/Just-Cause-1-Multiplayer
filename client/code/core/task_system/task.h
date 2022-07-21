#pragma once

class Task;

using task_ret_t		= bool;

using task_dispatch_t	= std::function<task_ret_t(Task*)>;
using task_completion_t = std::function<task_ret_t(Task*)>;

enum TaskState
{
	TaskState_Dispatching,
	TaskState_Completing,
	TaskState_Completed,
};

class Task
{
private:

	std::future<task_ret_t> f;

	task_dispatch_t dispatch;
	task_completion_t completion;

	std::atomic<TaskState> state = TaskState_Dispatching;

	bool async = false;

public:

	Task(const task_dispatch_t& dispatch, const task_completion_t& completion, bool async = true);
	~Task();

	/**
	* Checks the status of the task
	* @return Returns false if the task has been fully completed, true otherwise
	*/
	bool check();
	
	TaskState get_state() const { return state; }
};