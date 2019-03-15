
/*
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
#include <type_traits>
#include <vector>

#if defined(_OPENMP)
#include <omp.h>
#else
#pragma GCC warning \
    "OPENMP is not defined. Please compile with -fopenmp to get parallel for loops"
#endif

namespace test
{
struct dimension
{
  size_t row_dimen = 0;
  size_t col_dimen = 0;
  dimension() = default;
  dimension(size_t r, size_t c) : row_dimen(r), col_dimen(c) {}

  auto operator==(dimension const &b)
  {
    return this->row_dimen == b.row_dimen && this->col_dimen == b.col_dimen;
  }

  auto operator!=(dimension const &b) { return !(this->operator==(b)); }

  std::string to_string() const
  {
    return "[" + std::to_string(row_dimen) + "," + std::to_string(col_dimen) +
           "]";
  }

  size_t count() const { return row_dimen * col_dimen; }
};
namespace
{
struct util
{
  template <class expression_left_t, class expression_right_t>
  static size_t safe_index(expression_left_t const &expr1,
                           expression_right_t const &expr2,
                           size_t target)
  {
    if (std::is_same<decltype(expr1.get_format()),
                     decltype(expr2.get_format())>::value)
      return target;
    else
      return decltype(expr1.get_format())::to_other_major(
          target, expr1.get_dimension());
  }
  template <class expression_left_t, class expression_right_t>
  static void assert_same_dimensions(expression_left_t const &a,
                                     expression_right_t const &b)
  {
    if (a.get_dimension() != b.get_dimension())
      throw std::logic_error(
          std::string(
              "Dimension assertion failed. Required same dimensions found ") +
          a.get_dimension().to_string() + std::string(" and ") +
          b.get_dimension().to_string());
  }
};
} // namespace

namespace policy
{
template <class value_t>
struct RowMajorPolicy
{
  static auto &ordering(std::vector<value_t> &bucket, size_t indexX,
                        size_t indexY, dimension dimen)
  {
    return bucket[(indexX * (dimen.col_dimen)) + (indexY)];
  }
  static auto ordering(std::vector<value_t> const &bucket, size_t indexX,
                       size_t indexY, dimension dimen)
  {
    return bucket[(indexX * (dimen.col_dimen)) + (indexY)];
  }
  static void fill(std::vector<value_t> &bucket,
                   std::vector<std::vector<value_t>> const &elems)
  {
    size_t counter = 0;
    for (auto &e : elems)
      for (auto &E : e)
        bucket[counter++] = E;
  }

  static size_t to_other_major(size_t const &i, dimension const &dimen)
  {
    size_t x = i / dimen.col_dimen;
    size_t y = i % dimen.col_dimen;
    return (y * dimen.row_dimen + x);
  }
};

template <class value_t>
struct ColumnMajorPolicy
{

  static auto &ordering(std::vector<value_t> &bucket,
                        size_t indexX,
                        size_t indexY,
                        dimension dimen)
  {
    return bucket[(indexY * (dimen.row_dimen)) + (indexX)];
  }

  static auto ordering(std::vector<value_t> const &bucket,
                       size_t indexX,
                       size_t indexY,
                       dimension dimen)
  {
    return bucket[(indexY * (dimen.row_dimen)) + (indexX)];
  }

  static void fill(std::vector<value_t> &bucket,
                   std::vector<std::vector<value_t>> const &elems)
  {
    size_t counter = 0;
    auto row_counts = elems.size();
    auto col_counts = elems[0].size();
#pragma omp parallel for
    for (size_t a = 0; a < col_counts; a++)
#pragma omp parallel for
      for (size_t b = 0; b < row_counts; b++)
        bucket[counter++] = elems[b][a];
  }

  static size_t to_other_major(size_t const &i, dimension const &dimen)
  {
    size_t y = i / dimen.row_dimen;
    size_t x = i % dimen.row_dimen;
    return (x * dimen.col_dimen + y);
  }
};
} // namespace policy

template <typename E>
class expression
{
public:
  auto get(size_t i) const
  {
    return static_cast<E const &>(*this).get(i);
  }

  auto get_dimension() const
  {
    return static_cast<E const &>(*this).get_dimension();
  }

  auto get_format() const { return static_cast<E const &>(*this).get_format(); }
};

template <typename value_t,
          class format_t = policy::RowMajorPolicy<value_t>,
          class storage_t = std::vector<value_t>>
class matrix final : public expression<matrix<value_t, format_t, storage_t>>
{
  dimension const _dimen;
  storage_t _elements;
  format_t _format;

  void _construct_container()
  {
    if (std::is_same<std::vector<value_t>, storage_t>::value)
    {
      _elements = std::move(std::vector<value_t>(_dimen.count()));
      // @todo(coder3101) : Make _dimen non-constant as it can change because
      // container is vector.I am not implementing mutable matrices. If one day we do, this
      // code needs to change.
    }
    else
    {
      if (_dimen.count() != _elements.size())
      {
        throw std::logic_error(
            std::string("Cannot Create a matrix out of provided array size. "
                        "Array size must be ") +
            std::to_string(_dimen.count()));
      }
    }
  }

  template <class expression_right_t>
  size_t _safe_index(expression_right_t &expr, size_t index) const
  {
    return util::safe_index(*this, expr, index);
  }

  template <typename expression_right_t>
  void _safe_copy(expression_right_t &expr, size_t i)
  {
    this->_elements[i] = expr.get(_safe_index(expr, i));
  }

  template <typename expression_right_t>
  void _safe_move(expression_right_t &&expr, size_t i)
  {
    this->_elements[i] = std::move(expr.get(_safe_index(expr, i)));
  }

public:
  auto get(size_t i, size_t j) const
  {
    return format_t::ordering(_elements, i, j, _dimen);
  }

  auto &get(size_t i, size_t j)
  {
    return format_t::ordering(_elements, i, j, _dimen);
  }

  auto get(size_t i) const { return _elements[i]; }

  auto &get(size_t i) { return _elements[i]; }

  auto get_dimension() const { return _dimen; }

  auto get_format() const { return _format; }

  matrix(size_t rc, size_t cc) : _dimen(rc, cc) { _construct_container(); }

  // cppcheck-suppress noExplicitConstructor
  matrix(std::initializer_list<std::initializer_list<value_t>> elem)
      : _dimen(elem.size(), elem.begin()->size())
  {
    _construct_container();
    for (auto e : elem)
    {
      if (e.size() != _dimen.col_dimen)
        std::logic_error(
            "Cannot create a matrix out of the provided initializer_list. "
            "Length of each initializer list must be same");
    }
    std::vector<std::vector<value_t>> res;
    for (auto &e : elem)
      res.push_back(e);
    format_t::fill(_elements, res);
  }

  // cppcheck-suppress noExplicitConstructor
  matrix(std::vector<std::vector<value_t>> elem)
      : _dimen(elem.size(), elem[0].size())
  {
    _construct_container();
    for (auto e : elem)
    {
      if (e.size() != _dimen.col_dimen)
        std::logic_error("Cannot create a matrix out of the provided vector of "
                         "vector.Lenght of each vector must be same");
    }
    format_t::fill(_elements, elem);
  }

  template <typename expression_sub_t>
  matrix(expression<expression_sub_t> const &expr) : _dimen(expr.get_dimension())
  {
    _construct_container();
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _safe_copy(expr, a);
  }

  matrix(matrix &&other) : _dimen(std::move(other.get_dimension()))
  {
    _construct_container();
#pragma omp parallel for
    for (size_t i = 0; i < this->_dimen.count(); i++)
      _safe_move(other, i);
  }

  template <typename expression_sub_t>
  matrix(expression<expression_sub_t> &&expr) : _dimen(expr.get_dimension())
  {
    _construct_container();
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _safe_move(expr, a);
  }

  template <typename expression_sub_t>
  auto &operator=(expression<expression_sub_t> const &expr)
  {
    util::assert_same_dimensions(*this, expr);
#pragma omp parallel for
    for (size_t i = 0; i < this->_dimen.count(); i++)
      _safe_copy(expr, i);
    return *this;
  }

  template <typename expression_sub_t>
  auto &operator=(expression<expression_sub_t> &&expr)
  {
    util::assert_same_dimensions(*this, expr);
#pragma omp parallel for
    for (size_t i = 0; i < this->_dimen.count(); i++)
      _safe_move(expr, i);
    return *this;
  }

  template <typename other_format_t, typename other_storage_t>
  auto &operator=(matrix<value_t, other_format_t, other_storage_t> const &other)
  {
    if (this != &other)
    {
      util::assert_same_dimensions(*this, other);
#pragma omp parallel for
      for (size_t i = 0; i < this->_dimen.count(); i++)
        _safe_copy(other, i);
      return *this;
    }
  }

  template <typename other_format_t, typename other_storage_t>
  auto &operator=(matrix<value_t, other_format_t, other_storage_t> &&other)
  {
    if (this != &other)
    {
      util::assert_same_dimensions(*this, other);
#pragma omp parallel for
      for (size_t i = 0; i < this->_dimen.count(); i++)
        _safe_move(other, i);
      return *this;
    }
  }

  template <typename expression_sub_t>
  matrix &operator+=(expression<expression_sub_t> const &expr)
  {
    util::assert_same_dimensions(*this, expr);
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _elements[a] += expr.get(_safe_copy(expr, a));
    return *this;
  }

  template <typename expression_sub_t>
  matrix &operator-=(expression<expression_sub_t> const &expr)
  {
    util::assert_same_dimensions(*this, expr);
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _elements[a] -= expr.get(_safe_copy(expr, a));
    return *this;
  }

  template <typename expression_sub_t>
  matrix &operator*=(expression<expression_sub_t> const &expr)
  {
    util::assert_same_dimensions(*this, expr);
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _elements[a] *= expr.get(_safe_copy(expr, a));
    return *this;
  }

  template <typename expression_sub_t>
  matrix &operator/=(expression<expression_sub_t> const &expr)
  {
    util::assert_same_dimensions(*this, expr);
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _elements[a] /= expr.get(_safe_copy(expr, a));
    return *this;
  }

  template <typename scalar_t>
  void scalar_add(scalar_t val)
  {
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _elements[a] = _elements[a] + val;
  }

  template <typename scalar_t>
  void scalar_sub(scalar_t val)
  {
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _elements[a] = _elements[a] - val;
  }

  template <typename scalar_t>
  void scalar_mul(scalar_t val)
  {
#pragma omp parallel for
    for (size_t a = 0; a < _dimen.count(); a++)
      _elements[a] = _elements[a] * val;
  }

  void view(std::ostream &stream = std::cout)
  {
    auto min_row = (10 > _dimen.row_dimen ? _dimen.row_dimen : 10);
    auto min_col = (10 > _dimen.col_dimen ? _dimen.col_dimen : 10);
#pragma omp parallel for
    for (size_t i = 0; i < min_row; i++)
    {
#pragma omp parallel for
      for (size_t j = 0; j < min_col; j++)
        stream << _elements.get(i, j) << " ";
      stream << "\n";
    }
  }
};

template <typename expression_sub_t>
bool operator==(expression<expression_sub_t> const &left_expr,
                expression<expression_sub_t> const &right_expr)
{
  if (left_expr.get_dimension() != right_expr.get_dimension())
    return false;
  for (size_t a = 0; a < left_expr.get_dimension().count(); a++)
    if (left_expr.get(a) != right_expr.get(util::safe_index(left_expr, right_expr, a)))
      return false;
  return true;
}

template <typename expression_left_t, typename expression_right_t>
class add_expr : public expression<add_expr<expression_left_t, expression_right_t>>
{
  expression_left_t const &_u;
  expression_right_t const &_v;

public:
  add_expr(expression_left_t const &u, expression_right_t const &v) : _u(u), _v(v)
  {
    util::assert_same_dimensions(u, v);
  }

  auto get(size_t i) const
  {
    return _u.get(i) + _v.get(util::safe_index(_u, _v, i));
  }

  auto get_dimension() const { return _u.get_dimension(); }

  auto get_format() const { return _u.get_format(); }
};

template <typename expression_left_t, typename expression_right_t>
class sub_expr : public expression<sub_expr<expression_left_t, expression_right_t>>
{
  expression_left_t const &_u;
  expression_right_t const &_v;

public:
  sub_expr(expression_left_t const &u, expression_right_t const &v) : _u(u), _v(v)
  {
    util::assert_same_dimensions(u, v);
  }

  auto get(size_t i) const
  {
    return _u.get(i) - _v.get(util::safe_index(_u, _v, i));
  }

  auto get_dimension() const { return _u.get_dimension(); }

  auto get_format() const { return _u.get_format(); }
};

template <typename expression_left_t, typename expression_right_t>
class mul_expr : public expression<mul_expr<expression_left_t, expression_right_t>>
{
  expression_left_t const &_u;
  expression_right_t const &_v;

public:
  mul_expr(expression_left_t const &u, expression_right_t const &v) : _u(u), _v(v)
  {
    util::assert_same_dimensions(u, v);
  }

  auto get(size_t i) const
  {
    return _u.get(i) * _v.get(util::safe_index(_u, _v, i));
  }

  auto get_dimension() const { return _u.get_dimension(); }

  auto get_format() const { return _u.get_format(); }
};

template <typename expression_left_t, typename expression_right_t>
class div_expr : public expression<div_expr<expression_left_t, expression_right_t>>
{
  expression_left_t const &_u;
  expression_right_t const &_v;

public:
  div_expr(expression_left_t const &u, expression_right_t const &v) : _u(u), _v(v)
  {
    util::assert_same_dimensions(u, v);
  }

  auto get(size_t i) const
  {
    return _u.get(i) / _v.get(util::safe_index(_u, _v, i));
  }

  auto get_dimension() const { return _u.get_dimension(); }

  auto get_format() const { return _u.get_format(); }
};

template <typename expression_left_t, typename expression_right_t>
auto operator+(expression_left_t const &u, expression_right_t const &v)
{
  return add_expr<expression_left_t, expression_right_t>(u, v);
}

template <typename expression_left_t, typename expression_right_t>
auto operator-(expression_left_t const &u, expression_right_t const &v)
{
  return sub_expr<expression_left_t, expression_right_t>(u, v);
}

template <typename expression_left_t, typename expression_right_t>
auto operator*(expression_left_t const &u, expression_right_t const &v)
{
  return mul_expr<expression_left_t, expression_right_t>(u, v);
}

template <typename expression_left_t, typename expression_right_t>
auto operator/(expression_left_t const &u, expression_right_t const &v)
{
  return div_expr<expression_left_t, expression_right_t>(u, v);
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

template <typename expression_left_t, typename expression_right_t>
auto operator|(expression_left_t const &u, expression_right_t const &v)
{
  if (u.get_dimension().col_dimen != v.get_dimension().row_dimen)
  {
    throw std::logic_error(
        std::string(
            "Dot product cannot be called on matrices with dimension ") +
        u.get_dimension().to_string() + " and " +
        v.get_dimension().to_string());
  }
  matrix<decltype(u.get(0, 0))> ans(u.get_dimension().row_dimen,
                                    v.get_dimension().col_dimen);
#pragma omp parallel for
  for (size_t i = 0; i < u.get_dimension().row_dimen; i++)
  {
#pragma omp parallel for
    for (size_t j = 0; j < v.get_dimension().col_dimen; j++)
    {
      ans.get(i, j) = 0;
#pragma omp parallel for
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
