#include <catch2/catch.hpp>
#include <memory>

struct Foo : public std::enable_shared_from_this<Foo> {
};

SCENARIO("lambda should be cheap", "[lambda]") {
    GIVEN("a shared instance") {
        auto foo = std::make_shared<Foo>();

        WHEN("capture the shared pointer in lambda and copy the lambda") {
            auto fn_1 = [shared = foo->shared_from_this()]() {};
            auto fn_2 = fn_1;

            fn_1();
            fn_2();

            REQUIRE(foo.use_count() == 3);
        }
    }
}
