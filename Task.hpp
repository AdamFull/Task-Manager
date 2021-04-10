#pragma once
#include "Containers.hpp"
#include "Hook.hpp"
#include <functional>
#include <mutex>
#include <condition_variable>
#include <list>

using worker_function = std::function<void(merge_data*)>;

class Task : public IDispatcher
{
public:
    Task();
    Task(const Task& ctask) = delete;
    Task(const worker_function& func, merge_data* tdata);
    ~Task();

    void update(merge_data* tdata);
    void update(const worker_function& func, merge_data* tdata);

    void stop();

    bool is_running() { return b_is_running; }
    bool is_data_avaliable() { return b_is_data_avaliable; }
    bool is_done() { return b_is_done; }
    bool is_fully_done() { return b_is_done && !b_is_data_avaliable; }

    void subscribe(ISubscriber *observer) override;
    void unsubscribe(ISubscriber *observer) override;
    void notify() override;

    std::shared_ptr<Task>& operator=(const std::shared_ptr<Task>& other) = delete;

private:
    void start();

private:
    bool b_is_running,
    b_is_data_avaliable,
    b_is_done,
    b_terminate;

    std::condition_variable cv_;
    std::mutex mutex_;

    merge_data* data;
    worker_function worker_func;

    std::list<ISubscriber*> observers;
};