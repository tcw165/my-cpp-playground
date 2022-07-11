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

/// Forward decl for overloaded impl.
void draw(const int &, std::ostream &, size_t);
void draw(const std::string &, std::ostream &, size_t);

namespace detail
{
  /// Runtime-polymorphism implementation

  class Drawable
  {
  public:
    /// Typed-overloaded ctor that uses polymorphic type for the polymorphic use.
    /// Note these ctor overload must not be "explicit"!
    template <typename T>
    Drawable(T actual) : self_(std::make_unique<PolymorphicDrawable<T>>(actual))
    {
      std::cerr << "ctor" << std::endl;
    }

    /// Copy ctor
    Drawable(const Drawable &other) : self_(other.self_->copy_())
    {
      std::cerr << "copy" << std::endl;
    }
    /// Move ctor
    Drawable(Drawable &&) noexcept = default;

    /// Copy-assignment operator
    Drawable &operator=(const Drawable &other)
    {
      return *this = Drawable(other);
    }
    /// Move-assignment operator
    Drawable &operator=(Drawable &&other) noexcept = default;

    /// Non-member function "draw" will have access to d's private members and
    /// allow the underlying draw_() call to call a non-member overloaded function.
    friend void draw(const Drawable &d, std::ostream &out, size_t position)
    {
      d.self_->draw_(out, position);
    }

  private:
    struct DrawableConcept
    {
      /// A virtual destructor allows destructing child class's instance on
      /// destructing a base class's pointer.
      virtual ~DrawableConcept() = default;
      virtual auto copy_() -> std::unique_ptr<DrawableConcept> const = 0;
      virtual void draw_(std::ostream &, size_t) const = 0;
    };

    template <typename T>
    struct PolymorphicDrawable final : DrawableConcept
    {
      PolymorphicDrawable(T model) : model_(std::move(model)) {}

      auto copy_() -> std::unique_ptr<DrawableConcept> const override
      {
        return std::make_unique<PolymorphicDrawable<T>>(*this);
      }

      void draw_(std::ostream &out, size_t position) const override
      {
        draw(model_, out, position);
      }

      T model_;
    };

  private:
    std::unique_ptr<DrawableConcept>
        self_;
  };
} // namespace detail

/// Use case

void draw(const std::vector<detail::Drawable> &drawables, std::ostream &out, size_t position = 0)
{
  out << "<drawable>" << std::endl;
  for (auto it = drawables.begin(); it != drawables.end(); ++it)
  {
    auto position = std::distance(drawables.begin(), it);
    draw(*it, std::cout << "  ", position);
  }
  out << "</drawable>" << std::endl;
}
/// Polymorphic drawing function for drawable int.
void draw(const int &i, std::ostream &out, size_t position)
{
  out << "[" << position << "] = " << i << " (int drawable)" << std::endl;
}
/// Polymorphic drawing function for drawable string.
void draw(const std::string &s, std::ostream &out, size_t position)
{
  out << "[" << position << "] = " << s << " (string drawable)" << std::endl;
}
/// Polymorphic drawing function for drawable string.
// This class doesn't inherit from any interface. With runtime-poly, you could
// plugin any class with a new overloading function for the polymorphic-use.
struct FooDrawable
{
};
void draw(const FooDrawable &s, std::ostream &out, size_t position)
{
  out << "[" << position << "] = FooDrawable" << std::endl;
}

SCENARIO("Simulate an engine that uses executors via runtime-polymorphism without inheritance", "[runtime-polymorphism]")
{
  GIVEN("A FL executor")
  {
    std::vector<detail::Drawable> drawables;

    drawables.push_back(0);
    // TODO(TC): Fix the infinite recursion.
    // drawables.push_back(drawables);
    drawables.push_back("hello");
    drawables.push_back(1);
    drawables.push_back(FooDrawable{});

    draw(drawables, std::cerr);
  }
}
