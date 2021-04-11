#pragma once
#include "observer.hpp"
#include <functional>
#include <mutex>
#include <condition_variable>
#include <list>

using worker_function = std::function<void(void*)>;

class routine : public IDispatcher, public std::enable_shared_from_this<routine>
{
public:
    routine();
    routine(const routine& croutine) = delete;
    routine(const worker_function& func, void* tdata);
    ~routine();

    void update(void* tdata);
    void update(const worker_function& func, void* tdata);

    void stop();

    bool is_running() { return b_is_running; }
    bool is_data_avaliable() { return b_is_data_avaliable; }
    bool is_done() { return b_is_done; }
    bool is_fully_done() { return b_is_done && !b_is_data_avaliable; }

    void subscribe(ISubscriber *observer) override;
    void unsubscribe(ISubscriber *observer) override;
    void notify() override;

    std::shared_ptr<routine>& operator=(const std::shared_ptr<routine>& other) = delete;
    bool operator==(const routine& lhs)
    {
        return lhs.data == this->data;
    }

private:
    void start();

private:
    bool b_is_running,
    b_is_data_avaliable,
    b_is_done,
    b_terminate;

    std::condition_variable cv_;
    std::mutex mutex_;

    void* data;
    worker_function worker_func;

    std::list<ISubscriber*> observers;
};