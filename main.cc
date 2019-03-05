/**
 * Copyright 2019 Ashar <ashar786khan@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "benchmark.hpp"
#include "include/matrix.hpp"
#include "normal_matrix.hpp"
#include <cassert>
#include <chrono>

/**
 * @brief A Lambda that returns the lazy_matrix with [row,col] and filled with v
 *
 */
auto get_lazy_matrix = [](size_t row, size_t col, int v) -> test::matrix_int {
  std::vector<std::vector<int>> data{row, std::vector<int>(col)};
  for (size_t t = 0; t < row; t++)
    for (size_t j = 0; j < col; j++)
      data[t][j] = v;
  return test::matrix_int(data);
};

/**
 * @brief A Lambda that returns a normat matrix of dimension [row, col] filled
 * with v
 *
 */
auto get_normal_matrix = [](size_t row, size_t col,
                            int v) -> test::Matrix<int> {
  std::vector<std::vector<int>> data{row, std::vector<int>(col)};
  for (size_t t = 0; t < row; t++)
    for (size_t j = 0; j < col; j++)
      data[t][j] = v;
  return test::Matrix<int>(data);
};

/**
 * @brief A Generic Lambda that computes a very long operation.
 *
 */
auto compute_something = [](auto &target, auto &scope) {
  target =
      scope + scope * scope + scope / scope + scope + scope + scope + scope +
      scope + scope + scope + scope / scope + scope + scope + scope + scope -
      scope + scope + scope + scope / scope + scope + scope + scope + scope +
      scope + scope + scope + scope / scope + scope + scope + scope + scope -
      scope + scope + scope + scope / scope + scope + scope + scope + scope +
      scope + scope + scope + scope / scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope +
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope +
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope +
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope +
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope +
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope +
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope + scope * scope + scope + scope + scope + scope -
      scope + scope + scope;
};

int main() {
  using test::benchmark;
  using test::matrix_int;
  using Matrix = test::Matrix<int>;

  // Block 1
  {
    matrix_int a = get_lazy_matrix(1000, 1000, 0);
    matrix_int b = get_lazy_matrix(1000, 1000, 10);

    Matrix a2 = get_normal_matrix(1000, 1000, 0);
    Matrix b2 = get_normal_matrix(1000, 1000, 10);

    auto result1 = benchmark::run("Execution with Expression template",
                                  [&]() { compute_something(a, b); });
    auto result2 = benchmark::run("Execution without Expression template",
                                  [&]() { compute_something(a2, b2); });
    result2.print_beautifully();
    result1.print_beautifully();
    assert(a2 == a);
    assert(a == a);
  }
  // Block 2
  {
    matrix_int a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    matrix_int b = {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}};
    Matrix a2 = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    Matrix b2 = {{7, 8, 9}, {4, 5, 6}, {1, 2, 3}};
    auto result = Matrix::dot(a2, b2);
    auto c = (a | b);
    assert(result == c);
  }

  return 0;
}
