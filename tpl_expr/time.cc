#include <algorithm>
#include <cassert>
#include <concepts>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <valarray>
#include <vector>

#include <Eigen/Dense>

#include <xtensor/xarray.hpp>

#include "dujour.hpp"

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

template <typename container_t>
void time_container_size(std::size_t Nx, container_t const &x,
                         container_t const &y, std::ostream &output) {
  double a = 1.1;
  double b = 1.2;

  double half = 0.5;
  auto f = [&](auto &&u) { return 4.2 * u; };

  constexpr std::size_t N = 5000;

  for (std::size_t _ = 0; _ < N; ++_) {
    auto __ = dujour::scope_timer<double>::start("abxpy_container");

    container_t fu = f(abxpy_container(a, b, x, y));

    assert(std::accumulate(std::begin(fu), std::end(fu), 0.) != 0.);
  }

  for (std::size_t _ = 0; _ < N; ++_) {
    auto __ = dujour::scope_timer<double>::start("abxpy_auto     ");

    container_t fu = f(abxpy_auto(a, b, x, y));

    assert(std::accumulate(std::begin(fu), std::end(fu), 0.) != 0.);
  }

  output << std::setw(7) << std::left << Nx;
  for (auto kv : dujour::scope_timer<double>::times()) {
    output << " " << kv.first << " " << std::setw(15) << kv.second.time.count();
  }
  output << std::endl;

  dujour::scope_timer<double>::clear();
}

template <typename container_t>
concept is_xarray = requires(container_t data) { data.shape(); };

template <typename container_t> void time_container(std::ostream &output) {
  std::vector<std::size_t> N = {50,     100,    250,     500,     1'000,
                                10'000, 50'000, 100'000, 500'000, 1'000'000};

  for (auto Nx : N) {
    std::valarray<double> x_ref(Nx), y_ref(Nx);
    std::iota(std::begin(x_ref), std::end(x_ref), 0.1);
    x_ref /= x_ref.size();
    y_ref = std::cos(x_ref) * std::cos(x_ref) / x_ref;

    // container_t x(Nx), y(Nx);
    container_t x, y;

    if constexpr (is_xarray<container_t>) {
      x.resize(std::vector<std::size_t>({Nx}));
      y.resize(std::vector<std::size_t>({Nx}));
    } else {
      x.resize(Nx);
      y.resize(Nx);
    }
    std::copy(std::begin(x_ref), std::end(x_ref), std::begin(x));
    std::copy(std::begin(y_ref), std::end(y_ref), std::begin(y));

    time_container_size(Nx, x, y, output);
  }
}

#define STRINGIFY(x) #x

#define TIME_CONTAINER(container_t, label)                                     \
  {                                                                            \
    std::cout << #container_t << std::flush;                                   \
    std::ofstream output(STRINGIFY(label) ".txt");                             \
    std::cout << ".." << std::flush;                                           \
    time_container<container_t>(output);                                       \
    std::cout << "." << std::endl;                                             \
  }

int main() {

  /*
    { std::cout << "xt::xarray<double>";
      std::ofstream output("xarray.txt");
      std::cout << "..";
      time_container<xt::xarray<double>>(output);
      std::cout << "." << std::endl;
    }

    {
      std::ofstream output("valarray.txt");
      time_container<std::valarray<double>>(output);
    }

    {
      std::ofstream output("vectorXd.txt");
      time_container<Eigen::VectorXd>(output);
    }
    */

  TIME_CONTAINER(Eigen::VectorXd, vectorXd);
  TIME_CONTAINER(std::valarray<double>, valarray);
  TIME_CONTAINER(xt::xarray<double>, xarray);

  return 0;
}
