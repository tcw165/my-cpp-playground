#include <catch2/catch.hpp>
#include <iostream>

class Counter
{
public:
    explicit Counter(std::function<void()> ctor, std::function<void()> dtor) : ctor_(std::move(ctor)), dtor_(std::move(dtor))
    {
        ctor_();
    };

    ~Counter()
    {
        dtor_();
    };

    std::function<void()> ctor_;
    std::function<void()> dtor_;
};

class Foo
{
public:
    explicit Foo(std::function<void()> ctor, std::function<void()> dtor) : foo_(std::make_unique<Counter>(std::move(ctor), std::move(dtor))){};
    ~Foo(){};

    std::unique_ptr<Counter> foo_;
};

SCENARIO("class member is recycled automatically", "[Basic]")
{
    auto count = 0;
    auto increment = [&]
    {
        ++count;
    };
    auto decrement = [&]
    {
        --count;
    };

    {
        Foo tester(increment, decrement);
    }

    REQUIRE(count == 0);
}

SCENARIO("unamed object gets destroyed after eval", "[Basic]")
{
    auto count = 0;
    auto increment = [&]
    {
        ++count;
    };
    auto decrement = [&]
    {
        --count;
    };

    Counter{increment, decrement};
    REQUIRE(count == 0);
}