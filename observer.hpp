#pragma once
#include <memory>

class routine;

class ISubscriber {
 public:
  virtual ~ISubscriber(){};
  virtual void update(std::shared_ptr<routine> const& routine) = 0;
};

class IDispatcher {
 public:
  virtual ~IDispatcher(){};
  virtual void subscribe(ISubscriber *observer) = 0;
  virtual void unsubscribe(ISubscriber *observer) = 0;
  virtual void notify() = 0;
};