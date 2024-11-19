#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <numeric>
#include <valarray>

#include <Eigen/Dense>

#include <xtensor/xarray.hpp>
#include <xtensor/xfixed.hpp>

#include "dujour"

template <typename container_t, typename value_t>
auto abxpy_auto(value_t const &a, value_t const &b, container_t const &x,
                container_t const &y) {
  return a * (b * x) + y;
}

template <typename container_t, typename value_t>
container_t abxpy_container(value_t const &a, value_t const &b,
                            container_t const &x, container_t const &y) {
  return a * (b * x) + y;
}

template <typename container_t, typename func_t>
void time_container(container_t const &x, container_t const &y, func_t &f) {
  double a = 1.1;
  double b = 1.2;

  constexpr std::size_t N = 50000;

  for (std::size_t _ = 0; _ < N; ++_) {
    auto __ = dujour::scope_timer<double>::start("abxpy_container");

    container_t fu = f(abxpy_container(a, b, x, y));

    assert(std::accumulate(std::begin(fu), std::end(fu), 0.) == 0.);
  }

  for (std::size_t _ = 0; _ < N; ++_) {
    auto __ = dujour::scope_timer<double>::start("abxpy_auto");

    container_t fu = f(abxpy_auto(a, b, x, y));

    assert(std::accumulate(std::begin(fu), std::end(fu), 0.) == 0.);
  }
}

int main() {
  // prepare data
  static constexpr std::size_t Nx = 1000;
  std::valarray<double> x(Nx), y(Nx);
  std::iota(std::begin(x), std::end(x), 0.1);
  x /= x.size();
  y = std::cos(x) * std::cos(x) / x;

  double half = 0.5;

  auto f = [&](auto &&u) { return half * u - u + half * u; };

  std::cout << "std::valarray<double>:" << std::endl;
  {
    std::valarray<double> xx(Nx), yy(Nx);
    std::copy(std::begin(x), std::end(x), std::begin(xx));
    std::copy(std::begin(y), std::end(y), std::begin(yy));

    time_container(xx, yy, f);

    for (auto kv : dujour::scope_timer<double>::times()) {
      std::cout << kv.first << " " << kv.second.time.count() << " "
                << kv.second.count << "\n";
    }
  }

  dujour::scope_timer<double>::clear();

  std::cout << "\nxt::xarray<double>:" << std::endl;
  {
    xt::xarray<double> xx(Nx), yy(Nx);
    std::copy(std::begin(x), std::end(x), std::begin(xx));
    std::copy(std::begin(y), std::end(y), std::begin(yy));

    time_container(xx, yy, f);

    for (auto kv : dujour::scope_timer<double>::times()) {
      std::cout << kv.first << " " << kv.second.time.count() << " "
                << kv.second.count << "\n";
    }
  }

  dujour::scope_timer<double>::clear();

  std::cout << "\nEigen::VectorXd:" << std::endl;
  {
    Eigen::VectorXd xx(Nx), yy(Nx);
    std::copy(std::begin(x), std::end(x), std::begin(xx));
    std::copy(std::begin(y), std::end(y), std::begin(yy));

    time_container(xx, yy, f);

    for (auto kv : dujour::scope_timer<double>::times()) {
      std::cout << kv.first << " " << kv.second.time.count() << " "
                << kv.second.count << "\n";
    }
  }

  return 0;
}
