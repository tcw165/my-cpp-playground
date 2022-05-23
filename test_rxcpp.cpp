#include <catch2/catch.hpp>
#include <iostream>
#include <rxcpp/rx.hpp>

#include "papaya/papaya.hpp"

// SCENARIO("subscribe on a worker thread", "[rx]")
// {
//     std::cerr << "runner thread" << std::this_thread::get_id() << std::endl;

//     rxcpp::observable<>::create<int>([](rxcpp::subscriber<int> s) {
//                                          auto thread_id = std::this_thread::get_id();
//         std::cerr << "observable thread" << thread_id << std::endl;
//         s.on_next(0);
//         s.on_completed(); })
//         .subscribe_on(rxcpp::observe_on_new_thread())
//         .subscribe([](const auto i) {
//                        auto thread_id = std::this_thread::get_id();
//                        std::cerr << "observer thread" << thread_id << std::endl; });

//     std::cerr << "ready to return..." << std::endl;
//     std::this_thread::sleep_for(std::chrono::seconds(1));
// }

// rx-papaya prototype

// struct papaya
// {
//     void run()
//     {
//     }
// };

// auto do_fl_session() -> rxcpp::observable<bool>
// {
//     return rxcpp::observable<>::just(true);
// }

SCENARIO("prototype a rx-version of papaya", "[rx]")
{
    std::cerr << "runner thread" << std::this_thread::get_id() << std::endl;

    // auto subscriptions = rxcpp::composite_subscriptions();
    // subscriptions.add(
    //   do_fl_session()
    //     .subscribe_on(rxcpp::observe_on_new_thread())
    //     .subscribe([](const auto complete) {
    //         auto thread_id = std::this_thread::get_id();
    //         std::cerr << "observer thread" << thread_id << std::endl; })
    // );
    // // Later...
    // subscriptions.dispose();

    auto max_pending_request_size = 5ul;
    auto fl_factory = std::make_shared<pa::fl_factory>();
    pa::papaya papaya{max_pending_request_size, fl_factory};

    std::cerr << "ready to return..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
