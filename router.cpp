#include "router.hpp"
#include <thread>
#include <atomic>

std::atomic<router*> router::instance;
std::mutex router::imutex_;

router::router()
{
    max_routines_avaliable = std::thread::hardware_concurrency() - 1;
    for(size_t i = 0; i < max_routines_avaliable; i++)
    {
        auto pt = std::make_shared<routine>();
        pt->subscribe(this);
        free_routines.emplace_back(pt);
    }
}

router::~router()
{
    free_routines.clear();
    working_routines.clear();
}

void router::add_routine(worker_function func, void *data)
{
    if(free_routines.size() > 0)
    {
        //ресурс уже удалился из рабочих задач и не успел снова попасть во free_routines
        std::unique_lock<std::mutex> mu(omutex_);
        free_routines.front()->update(func, data);
        working_routines.emplace_back(free_routines.front());
        free_routines.erase(free_routines.begin());
        mu.unlock();
        return;
    }
    queued.emplace(func, data);
}

void router::kill_all()
{
    if(is_all_routines_done())
    {
        for(auto it : free_routines)
            it->stop();
    }
}

void router::update(std::shared_ptr<routine> const& nroutine)
{
    if(queued.size() > 0)
    {
        std::unique_lock<std::mutex> mu(omutex_);
        //SUPER CRITICAL PART START
        std::pair<worker_function, void*> paired = queued.front();
        nroutine->update(paired.first, paired.second);
        queued.pop();
        //SUPER CRITICAL PART END
        mu.unlock();
        return;
    }

    std::unique_lock<std::mutex> ms(omutex_);
    //SUPER CRITICAL PART START
    auto it = std::find_if(working_routines.begin(), working_routines.end(), [&](std::shared_ptr<routine> const& p) 
    {
        return *p == *nroutine;
    });
    free_routines.emplace_back(*it);
    working_routines.erase(it);
    //SUPER CRITICAL PART END
    ms.unlock();
}

router* router::GetInstance()
{
    router* routinemgr = instance.load(std::memory_order_acquire);
    if(!routinemgr)
    {
        std::lock_guard<std::mutex> lock(imutex_);
        routinemgr = instance.load(std::memory_order_relaxed);
        if(!routinemgr)
        {
            routinemgr = new router();
            instance.store(routinemgr, std::memory_order_release);
        }
    }
    return routinemgr;
}

bool router::is_all_routines_done()
{
    return working_routines.empty();
}

void router::wait()
{
    while(!is_all_routines_done());
}

size_t router::routines_number()
{
    return working_routines.size();
}