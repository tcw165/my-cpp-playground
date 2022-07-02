#include <catch2/catch.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <type_traits>
#include <utility>
#include <variant>

namespace detail
{
  template <typename T>
  class make_vector final
  {
  public:
    make_vector<T> &operator<<(T &&t)
    {
      // std::cerr << "operator<< for T" << std::endl;
      data_.emplace_back(std::forward<T>(t));
      // std::cerr << "---" << std::endl;
      return *this;
    }

    operator std::vector<T> &&()
    {
      std::cerr << "implicitly convert to std::vector<T>" << std::endl;
      return std::move(data_);
    }

  private:
    std::vector<T> data_;
  };

  struct Foo
  {
    inline static size_t constructCount = 0;
    inline static size_t moveCount = 0;

    Foo(int d) : data(d)
    {
      // std::cerr << "Foo(" << data << ")" << std::endl;
      ++Foo::constructCount;
    }
    Foo(Foo &&other) noexcept : data(other.data)
    {
      // std::cerr << "Foo(Foo&& " << data << ")" << std::endl;
      ++Foo::moveCount;
    }
    Foo(const Foo &other) = delete;

    const int data;

    friend std::ostream &operator<<(std::ostream &os, const Foo &self)
    {
      os << "Foo(" << self.data << ")";
      return os;
    }
  };
} // namespace detail

SCENARIO("Construct vector in place", "[vector]")
{
  WHEN("Call << for x1")
  {
    detail::Foo::constructCount = 0;
    detail::Foo::moveCount = 0;
    std::vector<detail::Foo> ints = detail::make_vector<detail::Foo>() << detail::Foo(1);

    REQUIRE(ints.size() == 1);
    REQUIRE(detail::Foo::constructCount == 1);
    REQUIRE(detail::Foo::moveCount >= 1);
  }

  WHEN("Call << for x2")
  {
    detail::Foo::constructCount = 0;
    detail::Foo::moveCount = 0;
    std::vector<detail::Foo> ints = detail::make_vector<detail::Foo>() << detail::Foo(1) << detail::Foo(2);

    REQUIRE(ints.size() == 2);
    REQUIRE(ints.at(0).data != ints.at(1).data);
    REQUIRE(detail::Foo::constructCount == 2);
    REQUIRE(detail::Foo::moveCount >= 2);
  }

  WHEN("Call << for x3")
  {
    detail::Foo::constructCount = 0;
    detail::Foo::moveCount = 0;
    std::vector<detail::Foo> ints = detail::make_vector<detail::Foo>() << detail::Foo(1) << detail::Foo(2) << detail::Foo(3);

    REQUIRE(ints.size() == 3);
    REQUIRE(ints.at(0).data != ints.at(1).data);
    REQUIRE(ints.at(1).data != ints.at(2).data);
    REQUIRE(detail::Foo::constructCount == 3);
    REQUIRE(detail::Foo::moveCount >= 3);
  }
}
