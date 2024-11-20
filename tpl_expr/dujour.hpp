#pragma once

#include <chrono>
#include <map>
#include <vector>

#ifndef DUJOUR_DEBUG
#define DUJOUR_DEBUG true
#endif

namespace dujour {

/**
 * @brief counter that accumulate time into a single duration
 *
 * @tparam T type for duration
 */
template <typename T> struct cumulative_counter {
  std::chrono::duration<T> time;
  std::size_t count;

  cumulative_counter() : time(std::chrono::duration<T>(0.)), count(0) {}

  void operator+=(std::chrono::duration<T> const &dt) {
    time += dt;
    ++count;
  }
};

/**
 * @brief counter that store each time into a vector to make statistics
 *
 * @tparam T type for duration
 */
template <typename T> struct violin_counter {
  std::vector<std::chrono::duration<T>> times;

  violin_counter() {}

  void operator+=(std::chrono::duration<T> const &dt) { times.push_back(dt); }
};

/**
 * @brief temporary structure that time its lifetime
 *
 * @tparam T         type for duration
 * @tparam counter_t type of counter (cumulative_counter or violin_counter)
 * @tparam clock_t   type of clock
 */
template <typename T, template <typename> typename counter_t,
          typename clock_t = std::chrono::high_resolution_clock>
struct ephemere {
  std::chrono::time_point<clock_t> start;
  counter_t<T> &store;

  ephemere(counter_t<T> &c) : start(clock_t::now()), store(c) {}

  ~ephemere() {
    auto end = clock_t::now();
    store += end - start;
  }
};

/**
 * @brief manager of timer
 *
 * @tparam T         type for duration
 * @tparam counter_t type of counter (cumulative_counter or violin_counter)
 * @tparam debug     boolean if true time code else no timers are not launch
 * @tparam clock_t   type of clock
 */
template <typename T,
          template <typename> typename counter_t = cumulative_counter,
          bool debug = DUJOUR_DEBUG,
          typename clock_t = std::chrono::high_resolution_clock>
struct scope_timer {};

/**
 * @brief specialization when debug is false (not extra cost)
 */
template <typename T, template <typename> typename counter_t, typename clock_t>
struct scope_timer<T, counter_t, false, clock_t> {
  static auto start(char const *label) { return false; }

  static std::map<char const *, counter_t<T>> times() {
    return std::map<char const *, counter_t<T>>();
  }

  static void clear() {}
};

/**
 * @brief specialization when debug is true, create a ephemere<T, counter_t,
 * clock_t> with start method
 */
template <typename T, template <typename> typename counter_t, typename clock_t>
struct scope_timer<T, counter_t, true, clock_t> {
  static std::map<char const *, counter_t<T>> _times;

  static auto start(char const *label) {
    return ephemere<T, counter_t, clock_t>(_times[label]);
  }

  static std::map<char const *, counter_t<T>> times() { return _times; }

  static void clear() { _times.clear(); }
};

template <typename T, template <typename> typename counter_t, typename clock_t>
std::map<const char *, counter_t<T>>
    scope_timer<T, counter_t, true, clock_t>::_times;

} // namespace dujour
