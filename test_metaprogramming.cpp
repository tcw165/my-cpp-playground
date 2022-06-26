#include <catch2/catch.hpp>
#include <functional>
#include <iostream>
#include <variant>

struct poly_visitor
{
    void operator()(const int &v)
    {
        common_visitor(v);
    }
    void operator()(const std::string &v)
    {
        common_visitor(v);
    }
    template <typename T>
    void common_visitor(const T &v)
    {
        // std::cerr << "visiting " << v << std::endl;
    }
};

template <typename T, typename VARIANT>
bool with_if(const VARIANT &variant, std::function<void(const T &)> &&runner)
{
    const auto *v_ptr = std::get_if<T>(&variant);
    if (v_ptr != nullptr)
    {
        runner(*v_ptr);
    }
    return v_ptr != nullptr;
};

SCENARIO("Visit std::variant with std::visit()", "[meta programming][runtime polymorphism]")
{
    UNSCOPED_INFO("XD");
    std::visit(poly_visitor{}, std::variant<int, std::string>(111));
    std::visit(poly_visitor{}, std::variant<int, std::string>("hello"));
}

SCENARIO("Visit std::variant with template function", "[meta programming][runtime polymorphism]")
{
    with_if<int>(std::variant<int, std::string>(111), [](const auto &v) { /* std::cerr << "visiting " << v << std::endl; */ });
}