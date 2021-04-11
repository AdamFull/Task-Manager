#include "TaskManager.hpp"
#include <thread>
#include <atomic>

//TODO: 
//shared_ptr problem
//observer problem
//deadlock problem
//ntdll problem

std::atomic<TaskManager*> TaskManager::instance;
std::mutex TaskManager::imutex_;

TaskManager::TaskManager()
{
    max_tasks_avaliable = std::thread::hardware_concurrency() - 1;
    //sem = new Semaphore(2);
    //working_tasks.reserve(max_tasks_avaliable);
    for(size_t i = 0; i < max_tasks_avaliable; i++)
    {
        auto pt = std::make_shared<Task>();
        pt->subscribe(this);
        free_tasks.emplace_back(pt);
    }
}

TaskManager::~TaskManager()
{
    free_tasks.clear();
    working_tasks.clear();
}

void TaskManager::add_task(worker_function func, merge_data *data)
{
    if(free_tasks.size() > 0)
    {
        free_tasks.front()->update(func, data);
        working_tasks.emplace_back(free_tasks.front());
        free_tasks.erase(free_tasks.begin());
        return;
    }
    queued.emplace(func, data);
}

void TaskManager::kill_all()
{
    if(is_all_tasks_done())
    {
        for(auto it : free_tasks)
            it->stop();
    }
}

void TaskManager::update(std::shared_ptr<Task> task)
{
    //Samoe ebanoe mesto kotoroe lomaet vse
    sem.notify(2);
    if(queued.size() > 0)
    {
        
        std::pair<worker_function, merge_data *> paired = queued.front();
        task->update(paired.first, paired.second);
        queued.pop();
        return;
    }

    auto it = std::find(working_tasks.begin(), working_tasks.end(), task);
    working_tasks.erase(it);
    free_tasks.emplace_back(task);
    sem.wait(2);
}

TaskManager* TaskManager::GetInstance()
{
    TaskManager* taskmgr = instance.load(std::memory_order_acquire);
    if(!taskmgr)
    {
        std::lock_guard<std::mutex> lock(imutex_);
        taskmgr = instance.load(std::memory_order_relaxed);
        if(!taskmgr)
        {
            taskmgr = new TaskManager();
            instance.store(taskmgr, std::memory_order_release);
        }
    }
    return taskmgr;
}

bool TaskManager::is_all_tasks_done()
{
    return working_tasks.empty();
}

void TaskManager::wait()
{
    while(!is_all_tasks_done());
}

size_t TaskManager::tasks_number()
{
    return working_tasks.size();
}