#pragma once

class Event 
{
    std::mutex mutex;
    std::condition_variable cv;

    bool signaled = false;

public:
    void signal(bool all = true) 
    {
        std::unique_lock <std::mutex> lock(mutex);
        signaled = true;

        if (all)
            cv.notify_all();
        else
            cv.notify_one();
    }

    void wait() 
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&]() { return signaled; });
        signaled = false;
    }
};