#pragma once

#include <queue>

namespace jc::thread_safe
{
	template <typename T>
	class queue
	{
	private:

		std::deque<T> data;
		std::condition_variable cond;
		std::mutex mtx;

		bool stopped = false;

	public:

		void push(const T& val)
		{
			std::lock_guard<std::mutex> lock(mtx);

			data.push(val);
			cond.notify_all();
		}

		void push(T&& val)
		{
			std::lock_guard<std::mutex> lock(mtx);

			data.push(val);
			cond.notify_all();
		}

		void stop()
		{
			std::lock_guard<std::mutex> lock(mtx);
			stopped = true;
			cond.notify_all();
		}

		bool wait_and_pop()
		{
			std::unique_lock<std::mutex> lock(mtx);

			cond.wait(lock, [this]() { return !data.empty() || stopped; });

			if (stopped)
				return false;

			data.pop();

			return true;
		}

		bool wait_and_pop(T& res)
		{
			std::unique_lock<std::mutex> lock(mtx);

			cond.wait(lock, [this]() { return !data.empty() || stopped; });

			if (stopped)
				return false;
			
			res = std::move(data.front());

			data.pop();

			return true;
		}

		T try_pop()
		{
			std::lock_guard<std::mutex> lock(mtx);

			if (data.empty())
				return {};

			auto res = std::move(data.front());

			data.pop();

			return res;
		}

		size_t size()
		{
			std::lock_guard<std::mutex> lock(mtx);
			return data.size();
		}

		bool empty()
		{
			std::lock_guard<std::mutex> lock(mtx);
			return data.empty();
		}

		bool is_stopped()
		{
			std::lock_guard<std::mutex> lock(mtx);
			return stopped;
		}
	};
}