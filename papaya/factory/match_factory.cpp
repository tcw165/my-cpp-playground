#include "papaya/factory/match_factory.hpp"

namespace pa
{

  match_factory::match_factory() {
    // no-op
  }

  auto match_factory::create(match::input input) -> rxcpp::observable<match::output>
  {
    return rxcpp::observable<>::just(match::output{});
  }

}