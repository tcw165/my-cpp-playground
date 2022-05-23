#pragma once

#include <rxcpp/rx.hpp>

namespace pa
{
  class fl_factory final
  {
  public:
    struct input
    {
    };
    struct output
    {
    };

  public:
    explicit fl_factory();
    auto create(fl_factory::input &input) -> rxcpp::observable<fl_factory::output>;
  };
}