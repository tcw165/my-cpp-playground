#include <catch2/catch.hpp>
#include <iostream>

class Counter
{
public:
    explicit Counter(std::function<void()> ctor, std::function<void()> dtor) : ctor_(std::move(ctor)), dtor_(std::move(dtor)) {
        std::cerr << "Counter() called" << std::endl;
        ctor_();
    };

    ~Counter() {
        std::cerr << "~Counter() called" << std::endl;
        dtor_();
    };

    std::function<void()> ctor_;
    std::function<void()> dtor_;
};

class Foo
{
public:
    explicit Foo(std::function<void()> ctor, std::function<void()> dtor) : foo_(std::make_unique<Counter>(std::move(ctor), std::move(dtor))) {
        std::cerr << "Foo() called" << std::endl;
    };
    ~Foo() {
        std::cerr << "~Foo() called" << std::endl;
    };

    std::unique_ptr<Counter> foo_;
};

SCENARIO("class member is recycled automatically", "[Basic]")
{
    auto count = 0;
    auto increment = [&] { ++count; std::cerr << "ctor() called" << std::endl; };
    auto decrement = [&] { --count; std::cerr << "dtor() called" << std::endl; };

    {
        Foo tester(increment, decrement);
    }

    REQUIRE(count == 0);
}

SCENARIO("unamed object gets destroyed after eval", "[Basic]")
{
    auto count = 0;
    auto increment = [&] { ++count; std::cerr << "ctor() called" << std::endl; };
    auto decrement = [&] { --count; std::cerr << "dtor() called" << std::endl; };

    std::cerr << "before unamed object instantiation" << std::endl;
    Counter{ increment, decrement };
    std::cerr << "after unamed object instantiation" << std::endl;
    REQUIRE(count == 0);
}