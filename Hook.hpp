#pragma once
#include <memory>

class Task;

class IObserver {
 public:
  virtual ~IObserver(){};
  virtual void update(std::shared_ptr<Task> task) = 0;
};

class ISubject : public std::enable_shared_from_this<Task> {
 public:
  virtual ~ISubject(){};
  virtual void subscribe(IObserver *observer) = 0;
  virtual void unsubscribe(IObserver *observer) = 0;
  virtual void notify() = 0;
};