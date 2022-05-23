#include "papaya.hpp"

#include <rxcpp/rx.hpp>

namespace pa
{

  namespace detail
  {
    // rxcpp::observable<>
  }

  papaya::papaya(
    size_t pending_request_size,
    std::shared_ptr<pa::fl_factory> fl_factory) 
    : pending_request_size_(pending_request_size),
      fl_factory_(fl_factory) {}

  papaya::~papaya() noexcept
  {
    this->stop();
  }

  bool papaya::run(
      input input,
      on_task_complete &&on_task_complete,
      on_run_error &&on_run_error,
      on_run_complete &&on_run_complete)
  {
    return false;
  }

  void papaya::stop()
  {
    // TODO: Stop background threads.cibtdvejvhguvgejdkncbtdtfhnubgti
  }
}