#pragma once

#include <rxcpp/rx.hpp>

namespace pa
{
  namespace match
  {
    struct input
    {
    };
    struct output
    {
    };
  }

  class match_factory final
  {
  public:
  public:
    explicit match_factory();
    auto create(match::input input) -> rxcpp::observable<match::output>;
  };
}