#pragma once
#include <memory>

class Task;

class ISubscriber {
 public:
  virtual ~ISubscriber(){};
  virtual void update(std::shared_ptr<Task> task) = 0;
};

class IDispatcher : public std::enable_shared_from_this<Task> {
 public:
  virtual ~IDispatcher(){};
  virtual void subscribe(ISubscriber *observer) = 0;
  virtual void unsubscribe(ISubscriber *observer) = 0;
  virtual void notify() = 0;
};