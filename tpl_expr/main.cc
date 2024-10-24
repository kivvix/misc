#include <algorithm>
#include <iostream>
#include <iterator>
#include <valarray>

#include <Eigen/Dense>

#include <xtensor/xarray.hpp>
#include <xtensor/xfixed.hpp>

template <typename container_t> void display(container_t const &data) {
  std::copy(std::begin(data), std::end(data),
            std::ostream_iterator<double>(std::cout, " "));
  std::cout << std::endl;
}

template <typename container_t, typename value_t>
auto axpy_by_value(value_t a, container_t const &x, container_t const &y) {
  return a * x + y;
}

template <typename container_t, typename value_t>
auto axpy_by_reference(value_t const &a, container_t const &x,
                       container_t const &y) {
  return a * x + y;
}

template <typename container_t, typename value_t>
auto abxpy_by_value(value_t a, value_t b, container_t const &x,
                    container_t const &y) {
  return a * b * x + y;
}

template <typename container_t, typename value_t>
auto abxpy_by_reference(value_t const &a, value_t const &b,
                        container_t const &x, container_t const &y) {
  return a * b * x + y;
}

template <typename container_t, typename value_t>
auto abxpy_by_reference2(value_t const &a, value_t const &b,
                         container_t const &x, container_t const &y) {
  return a * (b * x) + y;
}

template <typename container_t, typename value_t>
auto axbpy_by_reference(value_t const &a, value_t const &b,
                        container_t const &x, container_t const &y) {
  return a * x * b + y;
}

template <typename container_t> void test_container() {
  container_t x{{0., 1., 2.}};
  container_t y{{0., 1., 2.}};
  double a = 2.0;
  double b = 1.0;

  {
    std::cout << "[a by val]    a*x + y     \t";
    container_t z = axpy_by_value(a, x, y);
    display(z);
  }
  {
    std::cout << "[2 by val]    2*x + y     \t";
    container_t z = axpy_by_value(2.0, x, y);
    display(z);
  }
  {
    std::cout << "[a by ref]    a*x + y     \t";
    container_t z = axpy_by_reference(a, x, y);
    display(z);
  }
  {
    std::cout << "[2 by ref]    2*x + y     \t";
    container_t z = axpy_by_reference(2.0, x, y);
    display(z);
  }
  {
    std::cout << "[a, b by val] a*b*x + y   \t";
    container_t z = abxpy_by_value(a, b, x, y);
    display(z);
  }
  {
    std::cout << "[a, b by ref] a*b*x + y   \t";
    container_t z = abxpy_by_reference(a, b, x, y);
    display(z);
  }
  {
    std::cout << "[a, b by ref] a*(b*x) + y \t";
    container_t z = abxpy_by_reference2(a, b, x, y);
    display(z);
  }
  {
    std::cout << "[a, b by ref] a*x*b + y   \t";
    container_t z = axbpy_by_reference(a, b, x, y);
    display(z);
  }
}

int main() {
  std::cout << "std::valarray<double>:\n";
  test_container<std::valarray<double>>();

  std::cout << "\nEigen::Vector<double, 3>:\n";
  test_container<Eigen::Vector<double, 3>>();

  std::cout << "\nEigen::VectorXd:\n";
  test_container<Eigen::VectorXd>();

  std::cout << "\nxt::xarray<double>:\n";
  test_container<xt::xarray<double>>();

  std::cout << "\nxt::xtensor_fixed<double, xt::xshape<1, 3>>:\n";
  test_container<xt::xtensor_fixed<double, xt::xshape<1, 3>>>();

  std::cout << "\nxt::xtensor_fixed<double, xt::xshape<3, 1>>:\n";
  test_container<xt::xtensor_fixed<double, xt::xshape<3, 1>>>();
}
