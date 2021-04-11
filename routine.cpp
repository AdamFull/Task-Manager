#include "routine.hpp"
#include <thread>

routine::routine() : 
b_is_done(false), b_is_running(true), b_is_data_avaliable(false), b_terminate(false)
{
    std::thread(&routine::start, this).detach();
}

routine::routine(const worker_function &func, void *tdata) : 
b_is_done(false), b_is_running(true), b_is_data_avaliable(false), b_terminate(false)
{
    worker_func = func;
    data = tdata;
    std::thread(&routine::start, this).detach();
}

routine::~routine()
{
    stop();
}

void routine::update(void *tdata)
{
    if (!b_is_data_avaliable)
    {
        data = tdata;
        b_is_data_avaliable = true;
        b_is_done = false;
        cv_.notify_one();
    }
    else
        throw std::runtime_error("");
}

void routine::update(const worker_function &func, void *tdata)
{
    if (!b_is_data_avaliable)
    {
        data = tdata;
        worker_func = func;
        b_is_data_avaliable = true;
        b_is_done = false;
        cv_.notify_one();
    }
    else
        throw std::runtime_error("");
}

void routine::stop()
{
    b_is_running = false;
    b_terminate = true;
    b_is_data_avaliable = false;
    cv_.notify_one();
}

void routine::subscribe(ISubscriber *observer)
{
    observers.push_back(observer);
}

void routine::unsubscribe(ISubscriber *observer)
{
    observers.remove(observer);
}

void routine::notify()
{
    for(auto it : observers)
        it->update(shared_from_this());
}

void routine::start()
    {
        while(b_is_running)
        {
            std::unique_lock<decltype(mutex_)> lock(mutex_);
            cv_.wait(lock, [this]{ return b_is_data_avaliable; });

            if(b_terminate)
                return;

            if(b_is_data_avaliable)
            {
                worker_func(data);
                b_is_done = true;
                b_is_data_avaliable = false;
                notify();
            }
        }
    }