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

#include <benchmark.hpp>
#include <blas_matrix.hpp>
#include <cassert>
#include <chrono>
#include <normal_matrix.hpp>

auto get_iMatrix = [](size_t row, size_t col, int v) -> boost::test::iMatrix {
  std::vector<std::vector<int>> data{row, std::vector<int>(col)};
  for (int t = 0; t < row; t++)
    for (int j = 0; j < col; j++) data[t][j] = v;
  return boost::test::iMatrix(data);
};

auto get_NormalMatrix = [](size_t row, size_t col,
                           int v) -> boost::test::Matrix<int> {
  std::vector<std::vector<int>> data{row, std::vector<int>(col)};
  for (int t = 0; t < row; t++)
    for (int j = 0; j < col; j++) data[t][j] = v;
  return boost::test::Matrix<int>(data);
};

auto compute_sum = [](auto &target, auto &scope) {
  target =
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope + scope + scope + scope + scope + scope + scope +
      scope + scope + scope;
};

auto answer = [](auto target) {
  size_t row = target.get_dimension().row_dimen;
  size_t col = target.get_dimension().col_dimen;
  std::vector<std::vector<int>> data{row, std::vector<int>(col)};
  for (int t = 0; t < row; t++)
    for (int j = 0; j < col; j++) data[t][j] = 192;
  return boost::test::iMatrix(data);
};

auto answer2 = [](auto target) {
  size_t row = target.get_row();
  size_t col = target.get_col();
  std::vector<std::vector<int>> data{row, std::vector<int>(col)};
  for (int t = 0; t < row; t++)
    for (int j = 0; j < col; j++) data[t][j] = 192;
  return boost::test::Matrix<int>(data);
};
int main() {
  using boost::test::benchmark;
  using boost::test::iMatrix;
  using Matrix = boost::test::Matrix<int>;
  // Create the matrices a and b filled with 1 and 0
  iMatrix a = get_iMatrix(1000, 1000, 0);
  iMatrix b = get_iMatrix(1000, 1000, 1);

  Matrix a2 = get_NormalMatrix(1000, 1000, 0);
  Matrix b2 = get_NormalMatrix(1000, 1000, 1);

  // Run a benchmark with 192 length chained addition.
  benchmark::run("Execution with Expression template",
                 [&]() { compute_sum(a, b); })
      .print_beautifully();
  benchmark::run("Execution without Expression template",
                 [&]() { compute_sum(a2, b2); })
      .print_beautifully();

  assert(a2 == answer2(a2));
  assert(a == answer(a));

  // call .view() to see the contents of the matrix on stdout
  return 0;
}
