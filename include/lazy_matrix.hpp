

/*! \mainpage Matrix
 *
 * \section intro_sec Introduction
 *
 * Matrix is a Single header only Matrix Library for basic operations it
 * implements the expression template style to lazy-ly evaluate when it is
 * required. This Project was the part of boost.uBLAS Competancy Test in Google
 * Summer of Codes 2019 However you are free to use this any ways you want.
 *
 * \section install_sec Documentation
 *
 * Head over to this <a
 * href="https://coder3101.github.io/gsoc19-boost-test/html/classboost_1_1test_1_1matrix.html"><b>URL</b></a>
 * for the complete documentation of `matrix.hpp`
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
 * @brief This namespace holds the matrix library. It has been named so
 * because it is meant for boost.uBLAS Google Summer of Code 2019 Proposal
 * Competency test
 *
 */
namespace test {

/**
 * @brief This namespace contains all the policies that can be used with the
 * matrix class.
 *
 */
namespace policy {
/**
 * @brief Policy that specifies the Row Major Ordering to follow
 *
 * @tparam value_t the datatype of the matrix
 */
template <class value_t> struct RowMajorPolicy {
  /**
   * @brief Actual Implementation of the ordering
   *
   * @param bucket the flattened matrix representation vector.
   * @param indexX the row index that is requested
   * @param indexY the column index that is requested
   * @param rows the total rows in the matrix
   * @param column the total column in the matrix.
   * @return value_t& the reference to the element being selected by this
   * policy.
   */
  static auto &ordering(std::vector<value_t> &bucket, size_t indexX,
                        size_t indexY, size_t rows, size_t column) {
    return bucket[(indexX * (column)) + (indexY)];
  }
  /**
   * @brief Actual Implementation of the ordering
   *
   * @param bucket the flattened matrix representation vector.
   * @param indexX the row index that is requested
   * @param indexY the column index that is requested
   * @param rows the total rows in the matrix
   * @param column the total column in the matrix.
   * @return value_t& the reference to the element being selected by this
   * policy.
   */
  static auto ordering(std::vector<value_t> const &bucket, size_t indexX,
                       size_t indexY, size_t rows, size_t column) {
    return bucket[(indexX * (column)) + (indexY)];
  }
  /**
   * @brief Fills the bucket of flattened array using Row major Ordering
   *
   * @param bucket the flattened bucket to fill the elements to.
   * @param elems the elements in the form of vector of vectors.
   */
  static void fill(std::vector<value_t> &bucket,
                   std::vector<std::vector<value_t>> const &elems) {
    size_t counter = 0;
    for (auto &e : elems)
      for (auto &E : e)
        bucket[counter++] = E;
  }
};

/**
 * @brief Policy that specifies the Column Major Ordering to follow
 *
 * @tparam value_t the datatype of the matrix
 */
template <class value_t> struct ColumnMajorPolicy {
  /**
   * @brief Actual Implementation of the ordering
   *
   * @param bucket the flattened matrix representation vector.
   * @param indexX the row index that is requested
   * @param indexY the column index that is requested
   * @param rows the total rows in the matrix
   * @param column the total column in the matrix.
   * @return value_t& the reference to the element being selected by this
   * policy.
   */
  static auto &ordering(std::vector<value_t> &bucket, size_t indexX,
                        size_t indexY, size_t rows, size_t column) {
    return bucket[(indexY * (rows)) + (indexX)];
  }
  /**
   * @brief Actual Implementation of the ordering
   *
   * @param bucket the flattened matrix representation vector.
   * @param indexX the row index that is requested
   * @param indexY the column index that is requested
   * @param rows the total rows in the matrix
   * @param column the total column in the matrix.
   * @return value_t& the reference to the element being selected by this
   * policy.
   */
  static auto ordering(std::vector<value_t> const &bucket, size_t indexX,
                       size_t indexY, size_t rows, size_t column) {
    return bucket[(indexY * (rows)) + (indexX)];
  }

  /**
   * @brief Fills the bucket of flattened array using Column major Ordering
   *
   * @param bucket the flattened bucket to fill the elements to.
   * @param elems the elements in the form of vector of vectors.
   */
  static void fill(std::vector<value_t> &bucket,
                   std::vector<std::vector<value_t>> const &elems) {
    size_t counter = 0;
    auto row_counts = elems.size();
    auto col_counts = elems[0].size();
    for (size_t a = 0; a < col_counts; a++)
      for (size_t b = 0; b < row_counts; b++)
        bucket[counter++] = elems[b][a];
  }
};
} // namespace policy

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
template <typename E, class format_t> class expression {
public:
  /**
   * @brief returns the expression at i
   *
   * @param i the index
   * @return expression<E> the expression at that index.
   */
  auto get(size_t i) const {
    return static_cast<E const &>(*this).get(i); // East-Const
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
 *
 * @brief The template class of the matrix. It is final and implements
 * Curiously Recurring Templates Pattern. These matrices have immutable
 * dimension and shapes will not change once created.
 *
 * @tparam dtype the type that this matrix will hold.
 */
template <typename value_t, class format_t = policy::RowMajorPolicy<value_t>,
          class storage_t = std::vector<value_t>>
class matrix final : public expression<matrix<value_t>> {
  dimension const dimen;
  storage_t elements;

public:
  /**
   * @brief returns element at i, j position in the matrix.
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element at that position
   */
  auto get(size_t i, size_t j) const {
    return format_t::ordering(elements, i, j, dimen.row_dimen, dimen.col_dimen);
  }
  /**
   * @brief returns element at i, j position in the matrix by reference. It can
   * be used to assign values to i,j as well.
   *
   * @param i the row index
   * @param j the column index
   * @return dtype the element at that position
   */
  auto &get(size_t i, size_t j) {
    return format_t::ordering(elements, i, j, dimen.row_dimen, dimen.col_dimen);
  }
  /**
   * @brief Returns the value from the ith position in flat array
   *
   * @param i the index to look for
   * @return dtype the value returned
   */
  auto get(size_t i) const { return elements[i]; }

  /**
   * @brief Returns the value from the ith position in flat array by reference
   *
   * @param i the index to look for
   * @return dtype the value returned by reference
   */
  auto &get(size_t i) { return elements[i]; }

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
  matrix(size_t rc, size_t cc) : dimen(rc, cc), elements(rc * cc) {}
  /**
   * @brief Construct a new matrix object from an initializer list.
   *
   * @param elem the initializer list of initialiazer lists
   */
  // cppcheck-suppress noExplicitConstructor
  matrix(std::initializer_list<std::initializer_list<value_t>> elem)
      : dimen(elem.size(), elem.begin()->size()),
        elements(elem.size() * elem.begin()->size()) {
    for (auto e : elem) {
      if (e.size() != dimen.col_dimen)
        std::logic_error(
            "Cannot create a matrix out of the provided initializer_list. "
            "Length of each initializer list must be same");
    }
    std::vector<std::vector<value_t>> res;
    for (auto &e : elem)
      res.push_back(e);
    format_t::fill(elements, res);
  }
  /**
   * @brief Construct a new lazy matrix object from vectors
   *
   * @param elem vectors of vectors of elements.
   */
  // cppcheck-suppress noExplicitConstructor
  matrix(std::vector<std::vector<value_t>> elem)
      : dimen(elem.size(), elem[0].size()),
        elements(elem.size() * elem[0].size()) {
    for (auto e : elem) {
      if (e.size() != dimen.col_dimen)
        std::logic_error("Cannot create a matrix out of the provided vector of "
                         "vector.Lenght of each vector must be same");
    }
    format_t::fill(elements, elem);
  }

  /**
   * @brief Construct a new matrix object from an expression type. The
   * Expression will be evaluated to initialize the new variable.
   *
   * @tparam E the expression template paramater
   * @param expr the expression to create the matrix from
   */

  template <typename E>
  matrix(expression<E> const &expr)
      : dimen(expr.get_dimension()),
        elements(dimen.row_dimen * dimen.col_dimen) {
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] = expr.get(a);
  }

  /**
   * @brief Assignment from a expression will cause the expression to be
   * evaluated.
   *
   * @tparam E the expression template type, should be deduced by compiler.
   * @param expr the expression from which to evaluate.
   * @return lazy_matrix& the reference to *this
   */

  template <typename E> matrix &operator=(expression<E> const &expr) {
    if (expr.get_dimension() != this->get_dimension()) {
      throw std::logic_error(std::string("Cannot assign. Dimensions are ") +
                             this->dimen.to_string() + std::string(" and ") +
                             expr.get_dimension().to_string());
    }
    for (size_t i = 0; i < this->dimen.row_dimen * this->dimen.col_dimen; i++)
      this->elements[i] = expr.get(i);
    return *this;
  }

  /**
   * @brief Assignment from other Matrix Variable.
   *
   * @param other the value with which to assign this variable.
   * @return lazy_matrix& the reference to *this
   */

  matrix &operator=(matrix const &other) {
    if (this != &other) {
      if (this->get_dimension() != other.get_dimension()) {
        throw std::logic_error(std::string("Cannot assign. Dimensions are ") +
                               this->dimen.to_string() + std::string(" and ") +
                               other.get_dimension().to_string());
      }
      for (size_t i = 0; i < this->dimen.row_dimen * this->dimen.col_dimen; i++)
        this->elements[i] = other.elements[i];
      return *this;
    }
  }

  /**
   * @brief Add and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return matrix& the reference to *this
   */

  template <typename E> matrix &operator+=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("+= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] += expr.get[a];
    return *this;
  }

  /**
   * @brief Subtract and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return matrix& the reference to *this
   */

  template <typename E> matrix &operator-=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("-= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] -= expr.get[a];
    return *this;
  }

  /**
   * @brief Multiply and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return matrix& the reference to *this
   */

  template <typename E> matrix &operator*=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("*= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] *= expr.get[a];
    return *this;
  }

  /**
   * @brief Division and Assignment overload with expression. It will cause
   * expression to be evaluated
   *
   * @tparam E the expression template paramater
   * @param expr the expression with which we will evaluate and assign to *this
   * @return matrix& the reference to *this
   */

  template <typename E> matrix &operator/=(expression<E> const &expr) {
    if (this->dimen != expr.get_dimension()) {
      throw std::logic_error(
          std::string("/= operator not permitted. Dimensions are ") +
          this->dimen.to_string() + std::string(" and ") +
          expr.get_dimension().to_string());
    }
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] /= expr.get[a];
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
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      if (elements[a] != expr.get(a))
        return false;
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
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] += t;
  }

  /**
   * @brief Scaler subtraction to all the elements in the matrix.
   * Type must have an overload with - operator to dtype of matrix.
   *
   * @tparam T the type to subtract.
   * @param t the value to subtracted from all element.
   */

  template <typename T> void scalar_sub(int t) {
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] -= t;
  }

  /**
   * @brief Scaler Multiplication to all the elements in the
   * matrix. Type must have an overload with * operator to dtype of matrix.
   *
   * @tparam T the type to multiply.
   * @param t the value to multiply to all element.
   */

  template <typename T> void scalar_mul(T t) {
    for (size_t a = 0; a < dimen.row_dimen * dimen.col_dimen; a++)
      elements[a] *= t;
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
        stream << matrix(i, j) << " ";
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
   * @brief returns the value at i in the result
   *
   * @param i the row index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i) const { return _u.get(i) + _v.get(i); }

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
   * @brief returns the value at i in the result
   *
   * @param i the row index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i) const { return _u.get(i) - _v.get(i); }

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
   * @brief returns the value at i in the result
   *
   * @param i the row index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i) const { return _u.get(i) * _v.get(i); }
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
   * @brief returns the value at i in the result
   *
   * @param i the row index
   * @return dtype the element of the matrix or expression
   */

  auto get(size_t i) const { return _u.get(i) / _v.get(i); }
  /**
   * @brief Get the dimension of this expression object.
   *
   * @return dimension of the expression.
   */
  auto get_dimension() const { return _u.get_dimension(); }
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

/*
@NOTICE :
This dot product code cannot be very much optimized. I am forced to access the
the elements in a matrix form to actually do the dot product. However I found
that if two operands have different ordering {i.e Row and Column} we may in fact
be able to sequencially access the values. However if the two have same ordering
{say Row and Row} the rules of algebra forces us to evaluate the product by
accessing non adjacent memory blocks. For Now I keep it same as old.
*/

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
  matrix<decltype(u.get(0, 0))> ans(u.get_dimension().row_dimen,
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
using matrix_int = matrix<int>;
using matrix_long = matrix<long long>;
using matrix_float = matrix<float>;
using matrix_double = matrix<double>;
using matrix_complex_float = matrix<std::complex<float>>;
using matrix_complex_double = matrix<std::complex<double>>;
using matrix_complex_long = matrix<std::complex<long long>>;

} // namespace test
#endif
