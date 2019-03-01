

/*! \mainpage Lazy Matrix
 *
 * \section intro_sec Introduction
 *
 * Lazy Matrix is a Single header only Matrix Library for basic operations it
 * implements the expression template style to lazy-ly evaluate when it is
 * required. This Project was the part of boost.uBLAS Competancy Test in Google
 * Summer of Codes 2019 However you are free to use this any ways you want.
 *
 * \section install_sec Documentation
 *
 * Head over to this <a
 * href="https://coder3101.github.io/gsoc19-boost-test/html/classboost_1_1test_1_1lazy__matrix.html"><b>URL</b></a>
 * for the complete documentation of `lazy_matrix.hpp`
 *
 * You can also read Sample Examples at <a
 * href="https://github.com/coder3101/gsoc19-boost-test">this url</a>
 *
 * \section license License
 *
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
 *
 */

#ifndef LAZY_MATRIX_HPP
#define LAZY_MATRIX_HPP

#include <complex>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief PRINT_ROW_LIMIT
 * This MACRO sets how much of row to print in the call to matrix.view()
 * Defaults to 10 Rows.
 */
#ifndef PRINT_ROW_LIMIT
#define PRINT_ROW_LIMIT (10)
#endif

/**
 * @brief PRINT_COL_LIMIT
 * This MACRO sets how much of column to print in the call to matrix.view()
 * Defaults to 10 Columns
 *
 */
#ifndef PRINT_COL_LIMIT
#define PRINT_COL_LIMIT (10)
#endif

/**
 * @brief This namespace holds the lazy matrix library. It has been named so
 * because it is meant for boost.uBLAS Google Summer of Code 2019 Proposal
 * Competency test
 *
 */
namespace boost::test {

/**
 * @brief This Structure holds the dimension of the Matrix. It overloads the
 * basic operators
 *
 */

struct dimension {
  /**
   * @brief the number of rows in the matrix
   *
   */
  size_t row_dimen = 0;
  /**
   * @brief the number of columns in the matrix
   *
   */
  size_t col_dimen = 0;
  /**
   * @brief Construct a new dimension object
   *
   */
  dimension() = default;
  /**
   * @brief Construct a new dimension object
   *
   * @param r The row of the matrix
   * @param c The column of the matrix
   */
  dimension(size_t r, size_t c) : row_dimen(r), col_dimen(c) {}
  /**
   * @brief Overload for the comparting of the two dimension object
   *
   * @param b the rhs aragument
   * @return bool returns true if lhs is equal to rhs
   */
  auto operator==(dimension const &b) {
    return this->row_dimen == b.row_dimen && this->col_dimen == b.col_dimen;
  }
  /**
   * @brief Overload for the comparaing of two dimension object
   *
   * @param b the rhs argument
   * @return bool returns true if lhs is not equal to rhs
   */
  auto operator!=(dimension const &b) { return !(this->operator==(b)); }
  /**
   * @brief Converts a dimension into a human readable string format.
   *
   * @return std::string
   */
  std::string to_string() const {
    return "[" + std::to_string(row_dimen) + "," + std::to_string(col_dimen) +
           "]";
  }
};

/**
 * @brief An template Expression class for a node in AST of lazy evalution.
 *
 * @tparam E the type to create an expression into.
 */
template <typename E> class expression {
public:
  /**
   * @brief returns the expression at i, j.
   *
   * @param i the row index
   * @param j the column index
   * @return expression<E> the expression at that index.
   */
  auto get(size_t i, size_t j) const {
    return static_cast<E const &>(*this).get(i, j); // East-Const
  }
  /**
   * @brief Get the dimension of expression object
   *
   * @return dimension the dimension of the expression.
   */
  dimension get_dimension() const {
    return static_cast<E const &>(*this).get_dimension();
  }
};

/**
 * @brief The template class of the lazy matrix. It is final and implements
 * Curiously Recurring Templates Pattern. These matrices have immutable
 * dimension and shapes will not change once created.
 *
 * @tparam dtype the type that this matrix will hold.
 */
template <typename dtype>
class lazy_matrix final : public expression<lazy_matrix<dtype>> {
  dimension const dimen;
  std::vector<std::vector<dtype>> matrix;

public:
  /**
   * @brief returns element at i, j position in the matrix.
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element at that position
   */
  auto get(size_t i, size_t j) const { return matrix[i][j]; }
  /**
   * @brief returns element at i, j position in the matrix by reference. It can
   * be used to assign values to i,j as well.
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element at that position
   */
  auto &get(size_t i, size_t j) { return matrix[i][j]; }
  /**
   * @brief Get the dimension of the matrix
   *
   * @return dimension the dimension of the matrix.
   */
  auto get_dimension() const { return dimen; }

  /**
   * @brief Construct a new lazy matrix object
   *
   * @param rc the rows in the matrix
   * @param cc the columns in the matrix
   */
  lazy_matrix(size_t rc, size_t cc)
      : dimen(rc, cc), matrix(rc, std::vector<dtype>(cc)) {}
  /**
   * @brief Construct a new lazy matrix object from an initializer list.
   *
   * @param elem the initializer list of initialiazer lists
   */
  // cppcheck-suppress noExplicitConstructor
  lazy_matrix(std::initializer_list<std::initializer_list<dtype>> elem)
      : dimen(elem.size(), elem.begin()->size()) {
    for (auto e : elem) {
      if (e.size() != dimen.col_dimen)
        std::logic_error(
            "Cannot create a matrix out of the provided initializer_list. "
            "Length of each initializer list must be same");
    }
    for (auto e : elem)
      matrix.push_back(e);
  }
  /**
   * @brief Construct a new lazy matrix object from vectors
   *
   * @param elem vectors of vectors of elements.
   */
  // cppcheck-suppress noExplicitConstructor
  lazy_matrix(std::vector<std::vector<dtype>> elem)
      : dimen(elem.size(), elem[0].size()) {
    for (auto e : elem) {
      if (e.size() != dimen.col_dimen)
        std::logic_error("Cannot create a matrix out of the provided vector of "
                         "vector.Lenght of each vector must be same");
    }
    for (auto e : elem)
      matrix.push_back(e);
  }

  /**
   * @brief Construct a new lazy matrix object from an expression type. The
   * Expression will be evaluated to initialize the new variable.
   *
   * @tparam E the expression template paramater
   * @param expr the expression to create the matrix from
   */

  template <typename E>
  lazy_matrix(expression<E> const &expr)
      : dimen(expr.get_dimension()),
        matrix(dimen.row_dimen,
               std::vector<decltype(expr.get(0, 0))>(dimen.col_dimen)) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] = expr.get(a, b);
    }
  }

  /**
   * @brief Assignment from a expression will cause the expression to be
   * evaluated.
   *
   * @tparam E the expression template type, should be deduced by compiler.
   * @param expr the expression from which to evaluate.
   * @return lazy_matrix& the reference to *this
   */

  template <typename E> lazy_matrix &operator=(expression<E> const &expr) {
    if (expr.get_dimension() != this->get_dimension()) {
      throw std::logic_error(std::string("Cannot assign. Dimensions are ") +
                             this->dimen.to_string() + std::string(" and ") +
                             expr.get_dimension().to_string());
    }
    for (size_t i = 0; i < this->dimen.row_dimen; i++)
      for (size_t j = 0; j < this->dimen.col_dimen; j++)
        this->matrix[i][j] = expr.get(i, j);
    return *this;
  }

  /**
   * @brief Assignment from other Matrix Variable.
   *
   * @param other the value with which to assign this variable.
   * @return lazy_matrix& the reference to *this
   */

  lazy_matrix &operator=(lazy_matrix const &other) {
    if (this != &other) {
      if (this->get_dimension() != other.get_dimension()) {
        throw std::logic_error(std::string("Cannot assign. Dimensions are ") +
                               this->dimen.to_string() + std::string(" and ") +
                               other.get_dimension().to_string());
      }
      for (size_t i = 0; i < this->dimen.row_dimen; i++)
        for (size_t j = 0; j < this->dimen.col_dimen; j++)
          this->matrix[i][j] = other.matrix[i][j];
    }
    return *this;
  }

  /**
   * @brief Add and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return lazy_matrix& the reference to *this
   */

  template <typename E> lazy_matrix &operator+=(expression<E> const &expr) {
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

  /**
   * @brief Subtract and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return lazy_matrix& the reference to *this
   */

  template <typename E> lazy_matrix &operator-=(expression<E> const &expr) {
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

  /**
   * @brief Multiply and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return lazy_matrix& the reference to *this
   */

  template <typename E> lazy_matrix &operator*=(expression<E> const &expr) {
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

  /**
   * @brief Division and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return lazy_matrix& the reference to *this
   */

  template <typename E> lazy_matrix &operator/=(expression<E> const &expr) {
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
  /**
   * @brief This operator will check if the expression and the matrix have same
   * value. It will cause the expression to be evalauted.
   *
   * @tparam E the template parameter of expression
   * @param expr the expresssion type to evaluate
   * @return true if the expresssion's evaluated value is same as this
   * @return false otherwise
   */

  template <typename E> bool operator==(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("== operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        if (matrix[a][b] != expr.get(a, b))
          return false;
    }
    return true;
  }

  /**
   * @brief Scaler Addition to all the elements in the matrix. Type
   * must have an overload with + operator to dtype of matrix.
   *
   * @tparam T the type to add.
   * @param t the value to add to all element.
   */

  template <typename T> void scalar_add(T t) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] += t;
    }
  }

  /**
   * @brief Scaler subtraction to all the elements in the matrix.
   * Type must have an overload with - operator to dtype of matrix.
   *
   * @tparam T the type to subtract.
   * @param t the value to subtracted from all element.
   */

  template <typename T> void scalar_sub(int t) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] -= t;
    }
  }

  /**
   * @brief Scaler Multiplication to all the elements in the
   * matrix. Type must have an overload with * operator to dtype of matrix.
   *
   * @tparam T the type to multiply.
   * @param t the value to multiply to all element.
   */

  template <typename T> void scalar_mul(T t) {
    for (size_t a = 0; a < dimen.row_dimen; a++) {
      for (size_t b = 0; b < dimen.col_dimen; b++)
        matrix[a][b] *= t;
    }
  }

  /**
   * @brief Prints the content of a matrix to the stream. Default to std::cout
   *
   * @param stream the stream to print the values.
   */

  void view(std::ostream &stream = std::cout) {
    auto min_row =
        (PRINT_ROW_LIMIT > dimen.row_dimen ? dimen.row_dimen : PRINT_ROW_LIMIT);
    auto min_col =
        (PRINT_COL_LIMIT > dimen.col_dimen ? dimen.col_dimen : PRINT_COL_LIMIT);

    for (size_t i = 0; i < min_row; i++) {
      for (size_t j = 0; j < min_col; j++)
        stream << matrix[i][j] << " ";
      stream << "\n";
    }
  }
};

/**
 * @brief Constructs a representation of node for add operation in the Abstract
 * Syntax Tree
 *
 * @tparam E1 the first operand type
 * @tparam E2 the second operand type
 */
template <typename E1, typename E2>
class add_expr : public expression<add_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

public:
  /**
   * @brief Construct a new add expr object
   *
   * @param u the first operand
   * @param v the second operand
   */
  add_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension() != v.get_dimension()) {
      throw std::logic_error(
          std::string("Cannot perform binary operation addition ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  /**
   * @brief returns the value at i, j in the result
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i, size_t j) const { return _u.get(i, j) + _v.get(i, j); }

  /**
   * @brief Get the dimension of this expression object.
   *
   * @return dimension of the expression.
   */

  auto get_dimension() const { return _u.get_dimension(); }
};

/**
 * @brief Constructs a representation of node for subtraction operation in the
 * Abstract Syntax Tree
 *
 * @tparam E1 the first operand type
 * @tparam E2 the second operand type
 */
template <typename E1, typename E2>
class sub_expr : public expression<sub_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

public:
  /**
   * @brief Construct a new sub expr object
   *
   * @param u the first operand
   * @param v the second operand
   */
  sub_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension() != v.get_dimension()) {
      throw std::logic_error(
          std::string("Cannot perform binary operation subtraction ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  /**
   * @brief returns the value at i, j in the result
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i, size_t j) const { return _u.get(i, j) - _v.get(i, j); }

  /**
   * @brief Get the dimension of this expression object.
   *
   * @return dimension of the expression.
   */
  auto get_dimension() const { return _u.get_dimension(); }
};

/**
 * @brief Constructs a representation of node for multiplication operation in
 * the Abstract Syntax Tree
 *
 * @tparam E1 the first operand type
 * @tparam E2 the second operand type
 */

template <typename E1, typename E2>
class mul_expr : public expression<mul_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

public:
  /**
   * @brief Construct a new multiplication expr object
   *
   * @param u the first operand
   * @param v the second operand
   */
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

  /**
   * @brief returns the value at i, j in the result
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i, size_t j) const { return _u.get(i, j) * _v.get(i, j); }
  /**
   * @brief Get the dimension of this expression object.
   *
   * @return dimension of the expression.
   */
  auto get_dimension() const { return _u.get_dimension(); }
};

/**
 * @brief Constructs a representation of node for div operation in the Abstract
 * Syntax Tree
 *
 * @tparam E1 the first operand type
 * @tparam E2 the second operand type
 */

template <typename E1, typename E2>
class div_expr : public expression<div_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

public:
  /**
   * @brief Construct a new div expr object
   *
   * @param u the first operand
   * @param v the second operand
   */
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

  /**
   * @brief returns the value at i, j in the result
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i, size_t j) const { return _u.get(i, j) / _v.get(i, j); }
  /**
   * @brief Get the dimension of this expression object.
   *
   * @return dimension of the expression.
   */
  auto get_dimension() const { return _u.get_dimension(); }
};
/**
 * @brief Depreacated and Should not be used. This operation was meant to
 * lazy-ly evaluate the dot product.
 *
 * @tparam E1 the type of the first operand
 * @tparam E2 the type of the second operand
 */
template <typename E1, typename E2>
class [[deprecated("This operation computes dot product using lazy methodolgy. "
                   "Please use the eager_dot instead which uses arranged for "
                   "loop and quickly computes the dot product")]] dot_expr
    : public expression<dot_expr<E1, E2>> {
  E1 const &_u;
  E2 const &_v;

public:
  /**
   * @brief Construct a new dot expr object
   *
   * @param u the first operand
   * @param v the second operand
   */
  dot_expr(E1 const &u, E2 const &v) : _u(u), _v(v) {
    if (u.get_dimension().col_dimen != v.get_dimension().row_dimen) {
      throw std::logic_error(
          std::string("Cannot perform binary operation dot product ") +
          std::string("matrices with different dimensions. Dimensions are ") +
          u.get_dimension().to_string() + std::string(" and ") +
          v.get_dimension().to_string());
    }
  }

  /**
   * @brief returns the value at i, j in the result
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i, size_t j) const {
    decltype(_u.get(0, 0)) ans = decltype(_u.get(0, 0))();
    for (int t = 0; t < _u.get_dimension().col_dimen; t++) {
      ans += _u.get(i, t) * _v.get(t, j);
    }
    return ans;
  }
  /**
   * @brief Get the dimension of this expression object.
   *
   * @return dimension of the expression.
   */
  auto get_dimension() const {
    dimension res;
    res.row_dimen = _u.get_dimension().row_dimen;
    res.col_dimen = _v.get_dimension().col_dimen;
    return res;
  }
};

/**
 * @brief Overload for Expression type. This operation returns a add_expr object
 * which can be evaluated to result it holds.
 *
 * @tparam E1 type of first operand
 * @tparam E2 type of second operand
 * @param u the first argument
 * @param v the second argument
 * @return add_expr<E1, E2> a proxy that represents a add operation.
 */

template <typename E1, typename E2>
add_expr<E1, E2> operator+(E1 const &u, E2 const &v) {
  return add_expr<E1, E2>(u, v);
}

/**
 * @brief Overload for Expression type. This operation returns a sub_expr object
 * which can be evaluated to result it holds.
 *
 * @tparam E1 type of first operand
 * @tparam E2 type of second operand
 * @param u the first argument
 * @param v the second argument
 * @return sub_expr<E1, E2> a proxy that represents a add operation.
 */

template <typename E1, typename E2>
sub_expr<E1, E2> operator-(E1 const &u, E2 const &v) {
  return sub_expr<E1, E2>(u, v);
}

/**
 * @brief Overload for Expression type. This operation returns a mul_expr object
 * which can be evaluated to result it holds.
 *
 * @tparam E1 type of first operand
 * @tparam E2 type of second operand
 * @param u the first argument
 * @param v the second argument
 * @return mul_expr<E1, E2> a proxy that represents a add operation.
 */

template <typename E1, typename E2>
mul_expr<E1, E2> operator*(E1 const &u, E2 const &v) {
  return mul_expr<E1, E2>(u, v);
}

/**
 * @brief Overload for Expression type. This operation returns a div_expr object
 * which can be evaluated to result it holds.
 *
 * @tparam E1 type of first operand
 * @tparam E2 type of second operand
 * @param u the first argument
 * @param v the second argument
 * @return div_expr<E1, E2> a proxy that represents a add operation.
 */

template <typename E1, typename E2>
div_expr<E1, E2> operator/(E1 const &u, E2 const &v) {
  return div_expr<E1, E2>(u, v);
}
/**
 * @brief Computes the dot product of two Matrices or expression and returns the
 * result matrix immediately.
 *
 * @tparam E1 the type of first operand
 * @tparam E2 the type of second operand
 * @param u the actual first argument
 * @param v the actual second argument
 * @return lazy_matrix the result matrix.
 */
template <typename E1, typename E2> auto operator|(E1 const &u, E2 const &v) {
  if (u.get_dimension().col_dimen != v.get_dimension().row_dimen) {
    throw std::logic_error(
        std::string(
            "Dot product cannot be called on matrices with dimension ") +
        u.get_dimension().to_string() + " and " +
        v.get_dimension().to_string());
  }
  lazy_matrix<decltype(u.get(0, 0))> ans(u.get_dimension().row_dimen,
                                         v.get_dimension().col_dimen);
  for (size_t i = 0; i < u.get_dimension().row_dimen; i++) {
    for (size_t j = 0; j < v.get_dimension().col_dimen; j++) {
      ans.get(i, j) = 0;
      for (size_t k = 0; k < v.get_dimension().row_dimen; k++)
        ans.get(i, j) += u.get(i, k) * v.get(k, j);
    }
  }
  return ans;
}

typedef lazy_matrix<int> matrix_int;
typedef lazy_matrix<long long> matrix_long;
typedef lazy_matrix<float> matrix_float;
typedef lazy_matrix<double> matrix_double;
typedef lazy_matrix<std::complex<float>> matrix_complex_float;
typedef lazy_matrix<std::complex<double>> matrix_complex_double;
typedef lazy_matrix<std::complex<long long>> matrix_complex_long;

} // namespace boost::test
#endif
