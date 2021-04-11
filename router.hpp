#pragma once
#include "routine.hpp"
#include <vector>
#include <queue>

class router : public ISubscriber
{
private:
    router();
    ~router();

    static std::atomic<router*> instance;
    static std::mutex imutex_;
    std::mutex omutex_;
public:
    router(router &other) = delete;
    

    void add_routine(worker_function func, void* data);
    void kill_all();
    bool is_all_routines_done();
    void wait();
    size_t routines_number();

    void update(std::shared_ptr<routine> const& routine) override;

    static router *GetInstance();
    router& operator=(const router&)= delete;

private:
    size_t max_routines_avaliable;
    std::vector<std::shared_ptr<routine>> free_routines;
    std::vector<std::shared_ptr<routine>> working_routines;
    std::queue<std::pair<worker_function, void*>> queued;
};