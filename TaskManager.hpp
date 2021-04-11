#pragma once
#include "Task.hpp"
#include <vector>
#include <queue>
#include "Semaphore.hpp"

class TaskManager : public ISubscriber
{
private:
    TaskManager();
    ~TaskManager();

    static std::atomic<TaskManager*> instance;
    static std::mutex imutex_;
    Semaphore sem;
public:
    TaskManager(TaskManager &other) = delete;
    

    void add_task(worker_function func, merge_data* data);
    void kill_all();
    bool is_all_tasks_done();
    void wait();
    size_t tasks_number();

    void update(std::shared_ptr<Task> task) override;

    static TaskManager *GetInstance();
    TaskManager& operator=(const TaskManager&)= delete;

private:
    size_t max_tasks_avaliable;
    std::vector<std::shared_ptr<Task>> free_tasks;
    std::vector<std::shared_ptr<Task>> working_tasks;
    std::queue<std::pair<worker_function, merge_data*>> queued;
};