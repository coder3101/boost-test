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

#pragma once

#include <boost/yap/expression.hpp>
#include <cassert>
#include <type_traits>
#include <vector>

namespace test
{
namespace yap
{

namespace details
{
template <typename value_t>
struct matrix_properties
{
  size_t rows;
  size_t cols;

  matrix_properties() = delete;
  matrix_properties(int a, int b) : rows(a), cols(b) {}
  matrix_properties(matrix_properties<value_t> &rhs) = default;
  matrix_properties(matrix_properties<value_t> &&rhs) = default;

  template <typename value_type_rhs>
  bool operator==(matrix_properties<value_type_rhs> const &rhs) const noexcept
  {
    return rhs.rows == this->rows && rhs.cols == this->cols &&
           std::is_same<value_type_rhs, value_t>::value;
  }
  inline size_t count() const noexcept { return rows * cols; }
};
} // namespace details

// Is always Row-major
template <typename value_t>
class matrix
{
  details::matrix_properties<value_t> _properties;
  std::vector<value_t> _elements;

public:
  matrix(size_t rows, size_t cols)
      : _elements(rows * cols), _properties(rows, cols) {}
  matrix(std::vector<value_t> vals, size_t rows, size_t cols)
      : _properties(rows, cols)
  {
    assert(vals.size() == rows * cols);
    for (auto e : vals)
      _elements.push_back(e);
  }
  auto operator[](size_t n) const { return _elements[n]; }
  auto &operator[](size_t n) { return _elements[n]; }
  auto at(size_t r, size_t c) const
  {
    return _elements[_properties.cols * r + c];
  }
  auto &at(size_t r, size_t c) { return _elements[_properties.cols * r + c]; }
  auto &get_properties() const { return _properties; }
};

struct expand_n
{
  template <typename value_t>
  auto operator()(boost::yap::expr_tag<boost::yap::expr_kind::terminal>,
                  matrix<value_t> const &mat)
  {
    return boost::yap::make_terminal(mat[n]);
  }
  std::size_t n;
};

template <typename value_t>
struct equal_sizes_impl
{
  auto operator()(boost::yap::expr_tag<boost::yap::expr_kind::terminal>,
                  matrix<value_t> const &mat)
  {
    auto const &expr_properties = mat.get_properties();
    value = expr_properties == properties;
    return 0;
  }
  details::matrix_properties<value_t> const &properties;
  bool value;
};

template <typename expression_t, typename value_t>
bool is_equal_dimension(details::matrix_properties<value_t> const &prop,
                        expression_t const &expr)
{
  equal_sizes_impl<value_t> impl{prop, true};
  boost::yap::transform(boost::yap::as_expr(expr), impl);
  return impl.value;
};

template <typename value_t, typename Expr>
matrix<value_t> &assign(matrix<value_t> &mat, Expr const &e)
{
  decltype(auto) expr = boost::yap::as_expr(e);
  decltype(auto) prop = mat.get_properties();
  assert(is_equal_dimension(prop, expr));
#pragma omp parallel for
  for (std::size_t i = 0; i < prop.count(); ++i)
  {
    mat[i] = boost::yap::evaluate(
        boost::yap::transform(boost::yap::as_expr(expr), expand_n{i}));
  }
  return mat;
}

template <typename value_t, typename Expr>
matrix<value_t> &operator+=(matrix<value_t> &mat, Expr const &e)
{
  decltype(auto) expr = boost::yap::as_expr(e);
  decltype(auto) prop = mat.get_properties();
  assert(is_equal_dimension(prop, expr));
#pragma omp parallel for
  for (std::size_t i = 0; i < prop.count(); ++i)
  {
    mat[i] += boost::yap::evaluate(
        boost::yap::transform(boost::yap::as_expr(expr), expand_n{i}));
  }
  return mat;
}

template <typename T>
struct is_matrix : std::false_type
{
};

template <typename value_t>
struct is_matrix<matrix<value_t>> : std::true_type
{
};

BOOST_YAP_USER_UDT_UNARY_OPERATOR(negate, boost::yap::expression,
                                  is_matrix); // -
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(multiplies, boost::yap::expression,
                                       is_matrix); // *
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(divides, boost::yap::expression,
                                       is_matrix); // /
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(modulus, boost::yap::expression,
                                       is_matrix); // %
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(plus, boost::yap::expression,
                                       is_matrix); // +
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(minus, boost::yap::expression,
                                       is_matrix); // -
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(less, boost::yap::expression,
                                       is_matrix); // <
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(greater, boost::yap::expression,
                                       is_matrix); // >
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(less_equal, boost::yap::expression,
                                       is_matrix); // <=
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(greater_equal, boost::yap::expression,
                                       is_matrix); // >=
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(equal_to, boost::yap::expression,
                                       is_matrix); // ==
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(not_equal_to, boost::yap::expression,
                                       is_matrix); // !=
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(logical_or, boost::yap::expression,
                                       is_matrix); // ||
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(logical_and, boost::yap::expression,
                                       is_matrix); // &&
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(bitwise_and, boost::yap::expression,
                                       is_matrix); // &
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(bitwise_or, boost::yap::expression,
                                       is_matrix); // |
BOOST_YAP_USER_UDT_ANY_BINARY_OPERATOR(bitwise_xor, boost::yap::expression,
                                       is_matrix); // ^

} // namespace yap

} // namespace test