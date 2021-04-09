#include "Task.hpp"
#include <thread>

Task::Task() : 
b_is_done(false), b_is_running(true), b_is_data_avaliable(false)
{
    std::thread(&Task::start, this).detach();
}

Task::Task(const worker_function &func, merge_data *tdata) : 
b_is_done(false), b_is_running(true), b_is_data_avaliable(false)
{
    worker_func = func;
    data = tdata;
    std::thread(&Task::start, this).detach();
}

Task::~Task()
{
    stop();
}

void Task::update(merge_data *tdata)
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

void Task::update(const worker_function &func, merge_data *tdata)
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

void Task::stop()
{
    b_is_running = false;
    cv_.notify_one();
}

void Task::subscribe(IObserver *observer)
{
    observers.push_back(observer);
}

void Task::unsubscribe(IObserver *observer)
{
    observers.remove(observer);
}

void Task::notify()
{
    for(auto it : observers)
        it->update(shared_from_this());
}

void Task::start()
    {
        while(b_is_running)
        {
            std::unique_lock<decltype(mutex_)> lock(mutex_);
            cv_.wait(lock, [this]{ return b_is_data_avaliable; });

            if(b_is_data_avaliable)
            {
                worker_func(data);
                b_is_done = true;
                b_is_data_avaliable = false;
                notify();
            }
        }
    }