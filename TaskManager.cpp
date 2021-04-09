#include "TaskManager.hpp"

TaskManager::TaskManager()
{
    max_tasks_avaliable = std::thread::hardware_concurrency() - 1;
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

void TaskManager::update(std::shared_ptr<Task> task)
{
    if(queued.size() > 0)
    {
        std::pair<worker_function, merge_data *> paired = queued.front();
        task->update(paired.first, paired.second);
        queued.pop();
        return;
    }
    working_tasks.erase(std::find(working_tasks.begin(), working_tasks.end(), task));
    free_tasks.emplace_back(task);
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