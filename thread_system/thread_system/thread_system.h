#pragma once

#define FUTURE_ARRAY(T, s) std::array<std::future<T>, s>

class Thread;

class ThreadSystem
{
private:

	std::unordered_set<Thread*> threads;

	void stop_threads_internal(Thread** list, size_t count);

	uint32_t main_thread_id = 0;

public:

	ThreadSystem();
	~ThreadSystem() {}

	static constexpr uint32_t MAX_THREADS() { return 128; }

	void pause();
	void suspend_all();
	void resume_all();

	template <typename F>
	void wait_future(F& f, bool make_invalid = false)
	{
		f.wait();

		if (make_invalid)
			f.get();
	}

	template <typename F, typename Fn, typename T = std::chrono::milliseconds>
	void wait_future(const F& f, const Fn& fn, const T& duration = 0ms)
	{
		do {
			if (fn)
				fn();

			pause();
		}
		while (f.wait_for(duration) != std::future_status::ready);
	}

	template <typename T, size_t S>
	void wait_futures(std::array<std::future<T>, S>& f_list)
	{
		for (auto& f : f_list)
			f.wait();
	}

	/**
	* Signals the cancellable sleep of the specified thread making it stop instantly in most of cases.
	* In the worse case it will wait a few milliseconds because of the work inside the thread.
	* This function will wait until the thread is completely stopped.
	* @param thread The thread to stop
	*/
	void remove_thread(Thread* thread);

	template <typename Fn, typename Cond>
	void call_until(const Fn& fn, Cond cond)
	{
		while (!cond())
			fn();
	}

	/**
	* Stops all the specified threads asynchronously.
	* Example: if we stop 2 threads it will cancel both and then wait.
	* @param targets The list of the threads to stop
	*/
	template <typename... A>
	void stop_threads(A&&... targets)
	{
		std::array<std::common_type_t<A...>, sizeof...(targets)> threads_array { std::forward<A>(targets)... };

		stop_threads_internal(threads_array.data(), threads_array.size());
	}

	void check_main_thread();

	template <typename T>
	bool check_future(T& f, bool make_invalid = false)
	{
		if (!f.valid())
			return false;

		const bool ready = f.wait_for(0ms) == std::future_status::ready;

		if (ready && make_invalid)
			f.get();

		return ready;
	}

	/**
	* Creates and runs a new thread
	* @param fn The start address of the thread
	* @return The created thread
	*/
	Thread* start_thread(const void* fn);
};

inline ThreadSystem* g_thread_system = nullptr;