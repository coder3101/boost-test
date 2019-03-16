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

#include <cassert>
#include <chrono>
#include "benchmark.hpp"
#include "include/yap_matrix.hpp"
#include "normal_matrix.hpp"

/**
 * @brief A Lambda that returns the yap_matrix with [row,col] and filled with v
 *
 */
auto get_lazy_matrix = [](size_t row, size_t col, int v) {
  return test::yap::matrix(std::vector<int>(row * col, v), row, col);
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
    for (size_t j = 0; j < col; j++) data[t][j] = v;
  return test::Matrix<int>(data);
};

/**
 * @brief A Generic Lambda that computes a very long operation.
 *
 */
auto compute_something_naive = [](auto &target, auto &scope) {
  target = scope + scope * scope + scope / scope + scope + scope + scope +
           scope + scope + scope + scope + scope + scope + scope + scope +
           scope + scope + scope / scope + scope + scope + scope + scope -
           scope + scope + scope + scope / scope + scope + scope + scope +
           scope + scope + scope + scope + scope / scope + scope + scope +
           scope + scope - scope + scope + scope + scope / scope + scope +
           scope + scope + scope + scope + scope + scope + scope / scope +
           scope + scope + scope + scope - scope + scope + scope +
           scope * scope + scope + scope + scope + scope;
};

auto compute_something_yap = [](auto &target, auto &scope) {
  test::yap::assign(
      target, scope + scope * scope + scope / scope + scope + scope + scope +
                  scope + scope + scope + scope + scope + scope + scope +
                  scope + scope + scope + scope / scope + scope + scope +
                  scope + scope - scope + scope + scope + scope / scope +
                  scope + scope + scope + scope + scope + scope + scope +
                  scope / scope + scope + scope + scope + scope - scope +
                  scope + scope + scope / scope + scope + scope + scope +
                  scope + scope + scope + scope + scope / scope + scope +
                  scope + scope + scope - scope + scope + scope +
                  scope * scope + scope + scope + scope + scope);
};

int main() {
  using test::benchmark;
  using Matrix = test::Matrix<int>;

  // Block 1
  {
    auto a = get_lazy_matrix(2000, 2000, 0);
    auto b = get_lazy_matrix(2000, 2000, 10);

    auto a2 = get_normal_matrix(2000, 2000, 0);
    auto b2 = get_normal_matrix(2000, 2000, 10);

    auto result1 = benchmark::run("Execution using YAP Matrix",
                                  [&]() { compute_something_yap(a, b); });
    auto result2 = benchmark::run("Execution without Expression template",
                                  [&]() { compute_something_naive(a2, b2); });
    result2.print_beautifully();
    result1.print_beautifully();
    assert(result2 == result1);
    fflush(stdout);
  }

  return 0;
}
