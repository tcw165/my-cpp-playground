#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>

#include "papaya/factory/fl_factory.hpp"
#include "restrictions.hpp"

namespace pa
{

  class papaya
  {
  public:
    struct input
    {
      std::set<restriction> restrictions;
    };

    struct output
    {
      std::string task_name;
      std::unordered_map<std::string, std::variant<float, std::string>> metrics;
    };

    // Callback for specific task's complete. There might be more than one
    // tasks running in a run-session.
    using on_task_complete = std::function<void(output)>;
    // Callback for error in a run-session. When the error callback is called
    // the run session also stops. This callback is mutually exclusive with
    // complete callback.
    // For example, you might see a sequnce of call back like below:
    // task 1 > error
    using on_run_error = std::function<void(std::exception)>;
    // Callback for success of a run-session. For example, you might see a
    // sequence of callback like below:
    // task 1 > task 2 > complete
    using on_run_complete = std::function<void()>;

  public:
    // TODO: Inject deps using Fruit
    // TODO: Inject scheduler
    explicit papaya(
        size_t pending_request_size,
        std::shared_ptr<pa::fl_factory> fl_factory);
    ~papaya() noexcept;

    // Start a run-session.
    // If you start multiple runs before the previous run finishes,
    // saying \on_run_complete is called, they will be cached in a LRU cache
    // of size \pending_request_size_.
    // It returns true when your request is accepted; vice versa.
    bool run(
        input input,
        on_task_complete &&on_task_complete,
        on_run_error &&on_run_error,
        on_run_complete &&on_run_complete);
    void stop();

  private:
    const size_t pending_request_size_;
    std::shared_ptr<pa::fl_factory> fl_factory_;

    // TODO: declare a composite_subscription
  };

}