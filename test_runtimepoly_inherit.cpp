#include <atomic>
#include <catch2/catch.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <type_traits>
#include <variant>

namespace detail
{
  /// Abstract class that executes the primary training or evaluation loop.
  class IExecutor
  {
  public:
    struct Input final
    {
      std::string executionConfig;
      std::string dataset;
      std::string localInfo;
      std::string modelGraph;
      std::string dataDirectoryPath;
    };

    struct Success final
    {
    };

    struct Failure final
    {
      int32_t errorCode;
    };

    using Output = std::variant<Success, Failure>;

  public:
    virtual ~IExecutor() = default;

    virtual Output execute(const Input &) = 0;
  };

  /// FL (Federated Learning) executor
  class FlExecutor final : public IExecutor
  {
  public:
    FlExecutor() = default;
    // ~FlExecutor() = default;

    Output execute(const Input &input) override
    {
      return std::variant<Success, Failure>{Success{}};
    }
  };
} // namespace detail

SCENARIO("Simulate an engine that uses executors via inheritance based runtime-polymorphism", "[runtime-polymorphism]")
{
  GIVEN("A FL executor")
  {
    std::vector<std::unique_ptr<detail::IExecutor>> executors;

    executors.push_back(std::make_unique<detail::FlExecutor>());
  }
}
