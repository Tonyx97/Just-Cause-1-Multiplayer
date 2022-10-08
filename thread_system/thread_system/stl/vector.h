#pragma once

namespace jc::thread_safe
{
	template <typename T>
	class vector
	{
	private:

		mutable std::recursive_mutex mtx;

		std::vector<T> data;

		using data_const_it = typename decltype(data)::const_iterator;
		using data_it = typename decltype(data)::iterator;

	public:

		vector() {}
		vector(const vector& other)
		{
			std::lock_guard lock(other.mtx);
			data = other.data;
		}

		const T& push(const T& value)
		{
			std::lock_guard lock(mtx);
			data.push_back(value);
			return value;
		}

		void push(T&& value)
		{
			std::lock_guard lock(mtx);

			data.push_back(std::move(value));
		}

		bool erase(const T& value)
		{
			std::lock_guard lock(mtx);
			return std::erase_if(data, [&](const auto& v) { return v == value; }) > 0;
		}

		auto erase(data_const_it it)
		{
			std::lock_guard lock(mtx);
			return data.erase(it);
		}

		bool contains(const T& value)
		{
			std::lock_guard lock(mtx);
			return (std::find(data.begin(), data.end(), value) != data.end());
		}

		template <typename F>
		bool contains_if(const F& fn)
		{
			std::lock_guard lock(mtx);
			return (std::find_if(data.begin(), data.end(), fn) != data.end());
		}

		size_t size() const
		{
			std::lock_guard lock(mtx);
			return data.size();
		}

		bool empty() const
		{
			std::lock_guard lock(mtx);
			return data.empty();
		}

		void clear()
		{
			std::lock_guard lock(mtx);
			data.clear();
		}

		void lock()			{ mtx.lock(); }
		void unlock()		{ mtx.unlock(); }

		template <typename F>
		void for_each(const F& fn)
		{
			std::lock_guard lock(mtx);

			for (auto& e : data)
				fn(e);
		}

		template <typename Fn>
		void for_each_it(const Fn& fn)
		{
			std::lock_guard lock(mtx);

			for (auto it = data.begin(); it != data.end();)
				if (!fn(it))
					++it;
				else it = data.erase(it);
		}
	};
}