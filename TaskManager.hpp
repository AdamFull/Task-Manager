#pragma once
#include "Task.hpp"
#include <vector>
#include <queue>

class TaskManager : public IObserver
{
public:
    TaskManager();
    ~TaskManager();

    void add_task(worker_function func, merge_data* data);
    bool is_all_tasks_done();
    void wait();
    size_t tasks_number();

    void update(std::shared_ptr<Task> task) override;

private:
    size_t max_tasks_avaliable;
    std::vector<std::shared_ptr<Task>> free_tasks;
    std::vector<std::shared_ptr<Task>> working_tasks;
    std::queue<std::pair<worker_function, merge_data*>> queued;
};