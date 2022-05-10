#include <catch2/catch.hpp>
#include <iostream>

class Slave
{
public:
    explicit Slave(std::function<void()> ctor, std::function<void()> dtor) : ctor_(std::move(ctor)), dtor_(std::move(dtor)) {
        std::cerr << "Slave() called" << std::endl;
        ctor_();
    };

    ~Slave() {
        std::cerr << "~Slave() called" << std::endl;
        dtor_();
    };

    std::function<void()> ctor_;
    std::function<void()> dtor_;
};

class Host
{
public:
    explicit Host(std::function<void()> ctor, std::function<void()> dtor) : foo_(std::make_unique<Slave>(std::move(ctor), std::move(dtor))) {
        std::cerr << "Host() called" << std::endl;
    };
    ~Host() {
        std::cerr << "~Host() called" << std::endl;
    };

    std::unique_ptr<Slave> foo_;
};

TEST_CASE("memory management", "[Basic]")
{
    SECTION("if property is recycled automatically") {
        auto count = 0;
        auto increment = [&] { ++count; std::cerr << "ctor() called" << std::endl; };
        auto decrement = [&] { --count; std::cerr << "dtor() called" << std::endl; };

        {
            Host tester(increment, decrement);
        }

        REQUIRE(count == 0);
    };
}