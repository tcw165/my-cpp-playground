#include "papaya/factory/fl_factory.hpp"

// #include "match_factory.hpp"

namespace pa {

fl_factory::fl_factory() {
  // no-op
}
auto fl_factory::create(fl_factory::input& input) -> rxcpp::observable<fl_factory::output> {
  // TODO:
  //  match_factory_.create(match_input)
  //    | retry(...)
  //    | flat_map([](auto match_output) { return checkin_factory_.create(to_checkin_input(match_output)); })
  //    | flat_map([](auto checkin_output) { return download::proceed(to_download_input(checkin_output)); })
  //    ...etc
  return rxcpp::observable<>::just(fl_factory::output {});
}

}