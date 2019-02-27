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

#ifndef NORMAL_MATRIX_HPP
#define NORMAL_MATRIX_HPP

#include <iostream>
#include <vector>

namespace boost::test {
template <class T>
class Matrix {
  int row, col;

 public:
  std::vector<std::vector<T>> matrix;

  explicit Matrix(int r, int c)
      : row(r), col(c), matrix(row, std::vector<T>(col)){};
  // cppcheck-suppress noExplicitConstructor
  Matrix(std::initializer_list<std::initializer_list<T>> elem) {
    row = elem.size();
    col = *(elem.begin()).size();
    for (auto e : elem) matrix.push_back(e);
  }
  // cppcheck-suppress noExplicitConstructor
  Matrix(std::vector<std::vector<T>> elem) {
    row = elem.size();
    col = elem[0].size();
    for (auto e : elem) matrix.push_back(e);
  }

  Matrix operator+(const Matrix& other) const {
    Matrix res(row, col);
    for (int t = 0; t < row; t++)
      for (int j = 0; j < col; j++)
        res.matrix[t][j] = matrix[t][j] + other.matrix[t][j];
    return res;
  }

  Matrix operator-(const Matrix& other) const {
    Matrix res(row, col);
    for (int t = 0; t < row; t++)
      for (int j = 0; j < col; j++)
        res.matrix[t][j] = matrix[t][j] - other.matrix[t][j];
    return res;
  }
  Matrix operator*(const Matrix& other) const {
    Matrix res(row, col);
    for (int t = 0; t < row; t++)
      for (int j = 0; j < col; j++)
        res.matrix[t][j] = matrix[t][j] * other.matrix[t][j];
    return res;
  }
  Matrix operator/(const Matrix& other) const {
    Matrix res(row, col);
    for (int t = 0; t < row; t++)
      for (int j = 0; j < col; j++)
        res.matrix[t][j] = matrix[t][j] / other.matrix[t][j];
    return res;
  }

  bool operator==(const Matrix& other) const {
    for (int t = 0; t < row; t++)
      for (int j = 0; j < col; j++)
        if (matrix[t][j] != other.matrix[t][j]) return false;
    return true;
  }

  void view() const {
    for (int t = 0; t < row; t++) {
      for (int j = 0; j < col; j++) std::cout << matrix[t][j] << " ";
      std::cout << "\n";
    }
  }

  size_t get_row() const { return row; }
  size_t get_col() const { return col; }

  // todo (More operations will go here)
};
}  // namespace boost::test

#endif