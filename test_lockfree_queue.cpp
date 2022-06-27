#include <atomic>
#include <catch2/catch.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <type_traits>

namespace detail
{
  /**
   * @brief Thread-safe lock-free queue.
   *
   * @tparam T Trivially copyable type (guard by std::is_trivially_copyable).
   * @tparam size fixed size of the ring buffer under the hood.
   */
  template <typename T, size_t size>
  class ConcurrentFixedSizeQueue
  {
    static_assert(std::is_trivially_copyable<T>::value);
    // TODO: Apply a static-check for size > 0

  public:
    bool push(const T &newElement)
    {
      while (true)
      {
        auto old_write_pos = write_pos_.load();
        auto old_read_pos = read_pos_.load();
        auto new_write_pos = getPositionAfter(old_write_pos);
        // When new W and R points to the same slot, it's full.
        if (new_write_pos == old_read_pos)
        {
          return false;
        }

        if (write_pos_.compare_exchange_strong(old_write_pos, new_write_pos))
        {
          ring_buffer_[old_write_pos].store(newElement);
          return true;
        }
      }
    }

    bool pop(T &returnedElement)
    {
      while (true)
      {
        auto old_write_pos = write_pos_.load();
        auto old_read_pos = read_pos_.load();
        // When W and R points to the same slot, it's empty.
        if (old_write_pos == old_read_pos)
        {
          return false;
        }

        returnedElement = ring_buffer_[old_read_pos].load();

        if (read_pos_.compare_exchange_strong(old_read_pos, getPositionAfter(old_read_pos)))
        {
          return true;
        }
      }
    }

    bool is_empty()
    {
      return read_pos_.load() == write_pos_.load();
    }

    size_t effective_size()
    {
      auto read_pos = read_pos_.load();
      auto write_pos = write_pos_.load();
      if (write_pos >= read_pos)
      {
        return write_pos - read_pos;
      }
      else
      {
        return buffer_size_ - read_pos + write_pos;
      }
    }

  private:
    static constexpr size_t buffer_size_ = size + 1;
    std::atomic<size_t> read_pos_{0};
    std::atomic<size_t> write_pos_{0};
    std::array<std::atomic<T>, buffer_size_> ring_buffer_;

  private:
    template <typename FRIEND_T, size_t friend_size>
    friend class Verifier;

    /**
     * @brief Get the position after pos in the ring buffer.
     *
     * @param pos The current position.
     * @return size_t new position in the ring buffer.
     */
    static constexpr auto getPositionAfter(size_t pos) -> size_t
    {
      return ++pos == buffer_size_ ? 0 : pos;
    }
  };

  /**
   * @brief A helper to verify the content of ConcurrentFixedSizeQueue.
   */
  template <typename T, size_t size>
  class Verifier
  {
  public:
    /**
     * @brief Validate function that takes index and element.
     *
     * @param i The index of the element in testee's std::array.
     * @param element The element on the index.
     */
    template <typename TT>
    using ValidateElementFn = std::function<void(const size_t, const TT &)>;

    Verifier(const ConcurrentFixedSizeQueue<T, size> &queue, ValidateElementFn<T> ValidateFn)
    {
      auto read_pos = queue.read_pos_.load();
      auto write_pos = queue.write_pos_.load();
      for (auto i = 0; i < queue.ring_buffer_.size(); ++i)
      {
        if (write_pos > read_pos)
        {
          if (i >= read_pos && i < write_pos)
          {
            ValidateFn(i, queue.ring_buffer_[i]);
          }
        }
        else
        {
          if (i >= read_pos || i < write_pos)
          {
            ValidateFn(i, queue.ring_buffer_[i]);
          }
        }
      }
    }
  };
} // namespace detail

SCENARIO("Test dry pop", "[lock-free]")
{
  const auto test_size = 100;
  detail::ConcurrentFixedSizeQueue<int, test_size> q;

  WHEN("0 data in the queue and N concurrent reader reads in parallel")
  {
    std::vector<std::thread> all;
    for (auto i = 0; i < 10000; ++i)
    {
      try
      {
        std::thread reader([&]
                           {
        int read_element;
        q.pop(read_element); });
        all.push_back(std::move(reader));
      }
      catch (const std::system_error &ignored)
      {
        // no-op bc thread may not start due to low resource
      }
    }
    for (auto &t : all)
    {
      if (t.joinable())
      {
        t.join();
      }
    }

    THEN("the queue should still be empty")
    {
      REQUIRE(q.is_empty());
      REQUIRE(q.effective_size() == 0);
    }
  }
}

SCENARIO("Test when read position and write position are reversed", "[lock-free]")
{
  WHEN("1 writer pushes N elements in sequence")
  {
    const auto test_size = 3;
    detail::ConcurrentFixedSizeQueue<int, test_size> q;

    for (auto i = 0; i < test_size; ++i)
    {
      q.push(i);
    }

    THEN("All elements shall be unique")
    {
      REQUIRE(test_size == q.effective_size());
      detail::Verifier<int, test_size>(q, [cache = std::set<int>{}](const auto i, const auto &v) mutable
                                       {
      // std::cout << "[" << i << "]=" << v << std::endl;
      REQUIRE(0 == cache.count(v));
      cache.insert(v); });
    }

    THEN("read x2 and write x2 happens along")
    {
      int read_element;
      q.pop(read_element);
      q.pop(read_element);
      q.push(999);
      q.push(9999);

      AND_THEN("W pointer should be in front of W pointer")
      {
        detail::Verifier<int, test_size>(q, [cache = std::set<int>{}](const auto i, const auto &v)
                                         { 
                                          switch(i) {
                                            case 0: 
                                            REQUIRE(v == 9999);
                                            break;
                                            case 2: 
                                            REQUIRE(v == 2);
                                            break;
                                            case 3: 
                                            REQUIRE(v == 999);
                                            break;
                                          } });
      }

      REQUIRE_FALSE(q.is_empty());
      REQUIRE(q.effective_size() == test_size);
    }
  }
}

SCENARIO("Test 1-N concurrent interactions in a lock-free queue", "[lock-free]")
{
  WHEN("1 writer pushes N elements in sequence")
  {
    const auto test_size = 10000;
    detail::ConcurrentFixedSizeQueue<int, test_size> q;

    for (auto i = 0; i < test_size; ++i)
    {
      q.push(i);
    }

    THEN("All elements shall be unique")
    {
      REQUIRE(test_size == q.effective_size());
      detail::Verifier<int, test_size>(q, [cache = std::set<int>{}](const size_t i, const int &v) mutable
                                       {
      // std::cout << "[" << i << "]=" << v << std::endl;
      REQUIRE(0 == cache.count(v));
      cache.insert(v); });
    }

    THEN("N concurrent readers should exhaust the queue")
    {
      std::vector<std::thread> readers;
      try
      {
        for (auto i = 0; i < test_size; ++i)
        {
          std::thread t([&]
                        {
          int read_element;
          q.pop(read_element); });
          readers.push_back(std::move(t));
        }
      }
      catch (const std::system_error &ignored)
      {
        // no-op bc thread may not start due to low resource
      }
      for (auto &t : readers)
      {
        if (t.joinable())
        {
          t.join();
        }
      }

      REQUIRE(q.is_empty());
      REQUIRE(q.effective_size() == 0);
    }
  }

  WHEN("1 writer pushes N elements in sequence")
  {
    const auto test_size = 10000;
    detail::ConcurrentFixedSizeQueue<int, test_size> q;

    for (auto i = 0; i < test_size; ++i)
    {
      q.push(i);
    }

    THEN("2 * N concurrent readers should exhaust the queue")
    {
      std::vector<std::thread> readers;
      for (auto i = 0; i < 2 * test_size; ++i)
      {
        try
        {
          std::thread t([&]
                        {
            int read_element;
            q.pop(read_element); });
          readers.push_back(std::move(t));
        }
        catch (const std::system_error &ignored)
        {
          // no-op bc thread may not start due to low resource
        }
      }
      for (auto &t : readers)
      {
        if (t.joinable())
        {
          t.join();
        }
      }

      REQUIRE(q.is_empty());
      REQUIRE(q.effective_size() == 0);
    }
  }
}

SCENARIO("Test N-N concurrent interactions in a lock-free queue", "[lock-free]")
{
  WHEN("N concurrent writer pushes 1 elements individually in parellel")
  {
    const auto test_size = 10000;
    detail::ConcurrentFixedSizeQueue<int, test_size> q;

    std::vector<std::thread> writers;
    for (auto i = 0; i < test_size; ++i)
    {
      try
      {
        std::thread t([&, unique_i = i]
                      { q.push(unique_i); });
        writers.push_back(std::move(t));
      }
      catch (const std::system_error &ignored)
      {
        // no-op bc thread may not start due to low resource
      }
    }
    for (auto &t : writers)
    {
      if (t.joinable())
      {
        t.join();
      }
    }

    THEN("All elements shall be unique")
    {
      REQUIRE(test_size == q.effective_size());
      detail::Verifier<int, test_size>(q, [cache = std::set<int>{}](const size_t i, const int &v) mutable
                                       {
        // std::cout << "[" << i << "]=" << v << std::endl;
        REQUIRE(0 == cache.count(v));
        cache.insert(v); });

      AND_THEN("N concurrent readers that pop 1 element individually in parellel")
      {
        std::vector<std::thread> readers;
        for (auto i = 0; i < test_size; ++i)
        {
          try
          {
            std::thread t([&]
                          {
            int read_element;
            q.pop(read_element); });
            readers.push_back(std::move(t));
          }
          catch (const std::system_error &ignored)
          {
            // no-op bc thread may not start due to low resource
          }
        }
        for (auto &t : readers)
        {
          if (t.joinable())
          {
            t.join();
          }
        }

        REQUIRE(q.is_empty());
        REQUIRE(q.effective_size() == 0);
      }
    }
  }

  WHEN("N concurrent writer pushes N elements in total while N- concurrent readers read in parellel")
  {
    const auto stress_test_attmpts = 100;
    const auto test_size = 100;
    const auto thread_size = 4 * test_size;
    detail::ConcurrentFixedSizeQueue<int, test_size> q;
    for (auto i = 0; i < stress_test_attmpts; ++i)
    {
      GIVEN("Stree run #" << i)
      {
        std::vector<std::thread> all;
        for (auto i = 0; i < thread_size; ++i)
        {
          try
          {
            if (i < (size_t)(thread_size * 2.0 / 3.0))
            {
              std::thread writer([&, unique_i = i]
                                 { q.push(unique_i); });
              all.push_back(std::move(writer));
            }
            else
            {
              std::thread reader([&]
                                 {
                            int read_element;
                            q.pop(read_element); });
              all.push_back(std::move(reader));
            }
          }
          catch (const std::system_error &ignored)
          {
            // no-op bc thread may not start due to low resource
          }
        }
        for (auto &t : all)
        {
          if (t.joinable())
          {
            t.join();
          }
        }

        THEN("Queue may or may not be exhausted, but shouldn't be full")
        {
          REQUIRE(q.effective_size() < test_size);
          REQUIRE(q.effective_size() >= 0);
        }
      }
    }
  }
}