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

#ifndef BLAS_MATRIX_HPP
#define BLAS_MATRIX_HPP

#include <complex>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef PRINT_ROW_LIMIT
#define PRINT_ROW_LIMIT (30)
#endif

#ifndef PRINT_COL_LIMIT
#define PRINT_COL_LIMIT (30)
#endif

namespace boost::test {

struct dimension {
  size_t row_dimen = 0;
  size_t col_dimen = 0;
  dimension() = default;
  dimension(size_t r, size_t c) : row_dimen(r), col_dimen(c){};
  auto operator==(dimension const &b) {
    return this->row_dimen == b.row_dimen && this->col_dimen == b.col_dimen;
  }
  auto operator!=(dimension const &b) { return !(this->operator==(b)); }
  std::string to_string() const {
    return "[" + std::to_string(row_dimen) + "," + std::to_string(col_dimen) +
           "]";
  }
};

template <typename E>
class expression {
 public:
  auto get(size_t i, size_t j) const {
    return static_cast<E const &>(*this).get(i, j);  // East-Const
  }
  dimension get_dimension() const {
    return static_cast<E const &>(*this).get_dimension();
  }
};

template <typename dtype>
// Curiously Recurring template pattern
class blas_matrix final : public expression<blas_matrix<dtype>> {
  dimension dimen;
  std::vector<std::vector<dtype>> matrix;

 public:
  // Getters
  auto get(size_t i, size_t j) const { return matrix[i][j]; }
  auto &get(size_t i, size_t j) { return matrix[i][j]; }
  auto get_dimension() const { return dimen; }

  // Constructor
  blas_matrix(size_t rc, size_t cc)
      : dimension(rc, cc), matrix(rc, std::vector<dtype>(cc)){};

  // cppcheck-suppress noExplicitConstructor
  blas_matrix(std::initializer_list<std::initializer_list<dtype>> elem) {
    auto ff = *(elem.begin());
    size_t rxc = elem.size();
    size_t cxc = ff.size();
    for (auto e : elem) {
      if (e.size() != ff.size())
        std::logic_error(
            "Cannot create a matrix out of the provided initializer_list");
    }
    for (auto e : elem) matrix.push_back(e);
    dimen.col_dimen = cxc;
    dimen.row_dimen = rxc;
  }
  // cppcheck-suppress noExplicitConstructor
  blas_matrix(std::vector<std::vector<dtype>> elem) {
    auto cxc = elem[0].size();
    auto rxc = elem.size();
    for (auto e : elem) {
      if (e.size() != cxc)
        std::logic_error(
            "Cannot create a matrix out of the provided initializer_list");
    }
    for (auto e : elem) matrix.push_back(e);
    dimen.col_dimen = cxc;
    dimen.row_dimen = rxc;
  }

  template <typename E>
  // cppcheck-suppress noExplicitConstructor
  blas_matrix(expression<E> const &expr)
      : dimen(expr.get_dimension()),
        matrix(dimen.row_dimen,
               std::vector<decltype(expr.get(0, 0))>(dimen.col_dimen)) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] = expr.get(a, b);
    }
  }

  template <typename E>
  blas_matrix &operator+=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("+= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] += expr.get(a, b);
    }
    return *this;
  }

  template <typename E>
  blas_matrix &operator-=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("-= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] -= expr.get(a, b);
    }
    return *this;
  }

  template <typename E>
  blas_matrix &operator*=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("*= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] *= expr.get(a, b);
    }
    return *this;
  }

  template <typename E>
  blas_matrix &operator/=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("/= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] /= expr.get(a, b);
    }
    return *this;
  }

  template <typename E>
  bool operator==(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("== operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        if (matrix[a][b] != expr.get(a, b)) return false;
    }
    return true;
  }

  template <typename T>
  void scalar_add(T t) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++) matrix[a][b] += t;
    }
  }
  template <typename T>
  void scalar_sub(int t) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++) matrix[a][b] -= t;
    }
  }
  template <typename T>
  void scalar_mul(T t) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++) matrix[a][b] *= t;
    }
  }

  void view() {
    int printedX = 0;
    int printedY = 0;
    for (auto e : matrix) {
      if (printedY == PRINT_ROW_LIMIT) break;
      for (auto f : e) {
        printedX = 0;
        if (printedX == PRINT_COL_LIMIT) {
          std::cout << "...";
          break;
        }
        std::cout << f << " ";
        printedX++;
      }
      std::cout << "\n";
      printedY++;
    }
  }
};

template <typename E1, typename E2>
class add_expr : public expression<add_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

 public:
  add_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension() != v.get_dimension()) {
      throw std::logic_error(
          std::string("Cannot perform binary operation addition ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  auto get(size_t i, size_t j) const { return _u.get(i, j) + _v.get(i, j); };
  auto get_dimension() const { return _u.get_dimension(); }
};

template <typename E1, typename E2>
class sub_expr : public expression<sub_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

 public:
  sub_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension() != v.get_dimension()) {
      throw std::logic_error(
          std::string("Cannot perform binary operation subtraction ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  auto get(size_t i, size_t j) const { return _u.get(i, j) - _v.get(i, j); };
  auto get_dimension() const { return _u.get_dimension(); }
};

template <typename E1, typename E2>
class mul_expr : public expression<mul_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

 public:
  mul_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension() != v.get_dimension()) {
      throw std::logic_error(
          std::string("Cannot perform binary operation element wise "
                      "multiplication ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  auto get(size_t i, size_t j) const { return _u.get(i, j) * _v.get(i, j); };
  auto get_dimension() const { return _u.get_dimension(); }
};

template <typename E1, typename E2>
class div_expr : public expression<div_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

 public:
  div_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension() != v.get_dimension()) {
      throw std::logic_error(
          std::string(
              "Cannot perform binary operation element wise division ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  auto get(size_t i, size_t j) const { return _u.get(i, j) / _v.get(i, j); };
  auto get_dimension() const { return _u.get_dimension(); }
};

template <typename E1, typename E2>
class dot_expr : public expression<dot_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

 public:
  dot_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension().col_dimen != v.get_dimension().row_dimen) {
      throw std::logic_error(
          std::string("Cannot perform binary operation dot product ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  auto get(size_t i, size_t j) const {
    decltype(_u.get(0, 0)) ans = decltype(_u.get(0, 0))();
    for (int t = 0; t < _u.get_dimension().col_dimen; t++) {
      ans += _u.get(i, t) * _v.get(t, j);
    }
    return ans;
  };
  auto get_dimension() const {
    dimension res;
    res.row_dimen = _u.get_dimension().row_dimen;
    res.col_dimen = _v.get_dimension().col_dimen;
    return res;
  }
};

// Overloads
template <typename E1, typename E2>
add_expr<E1, E2> operator+(E1 const &u, E2 const &v) {
  return add_expr<E1, E2>(u, v);
}
template <typename E1, typename E2>
sub_expr<E1, E2> operator-(E1 const &u, E2 const &v) {
  return sub_expr<E1, E2>(u, v);
}
template <typename E1, typename E2>
mul_expr<E1, E2> operator*(E1 const &u, E2 const &v) {
  return mul_expr<E1, E2>(u, v);
}
template <typename E1, typename E2>
mul_expr<E1, E2> operator/(E1 const &u, E2 const &v) {
  return mul_expr<E1, E2>(u, v);
}
// | is a dot/matrix multiplication operator.
template <typename E1, typename E2>
dot_expr<E1, E2> operator|(E1 const &u, E2 const &v) {
  return dot_expr<E1, E2>(u, v);
}

// Some Outer Helper Functions

typedef blas_matrix<int> iMatrix;
typedef blas_matrix<long long> lMatrix;
typedef blas_matrix<float> fMatrix;
typedef blas_matrix<double> dMatrix;
typedef blas_matrix<std::complex<float>> cfMatrix;
typedef blas_matrix<std::complex<double>> cdMatrix;
typedef blas_matrix<std::complex<long long>> clMatrix;

}  // namespace boost::test
#endif