#pragma once

class CancellableSleep 
{
private:

	std::condition_variable cv;
	std::mutex mutex;
	std::function<bool()> fn;

    bool canceled = false;

	bool is_canceled()
	{
		return fn ? (canceled = fn()) : canceled;
	}

public:

	CancellableSleep() {}
	CancellableSleep(const std::function<bool()>& fn) : fn(fn) {}

    void cancel()
    {
        std::unique_lock<std::mutex> lock(mutex);
		
		if (fn)
			return;

        canceled = true;

        cv.notify_all();
    }

    template <typename T>
    bool sleep(T duration)
    {
        std::unique_lock<std::mutex> lock(mutex);

        cv.wait_for(lock, duration, [this]() { return is_canceled(); });

        return !std::exchange(canceled, false);
    }
};