#ifndef EXPRESSION_MATRIX
#define EXPRESSION_MATRIX

#include <boost/yap/print.hpp>
#include <boost/yap/yap.hpp>
#include <cassert>
#include <iostream>
#include <vector>

#if defined(_OPENMP)
#include <omp.h>
#else
#pragma GCC warning \
    "You can use -fopenmp to get parallel for loops that will improve the performance"
#endif

using namespace boost::hana::literals;

namespace details {

// As of now. std::execution does not works on my compiler. It is a workaround
// for now
enum device { Parallel = 1, Sequencial = 0 };

// Forward declare expression class
template <boost::yap::expr_kind Kind, typename Tuple>
struct yap_matrix_expression;
// Forward declare matrix class
template <typename value_t>
class yap_matrix;

// This is core of what will be contained in the AST of YAP.
// It must contain the container that is holding the values of tensor
// or matrix and its dimensions or extents, because while traversing the AST
// we need those properties to optimize the expression. This is what is
// contained in terminal nodes of the AST

template <typename value_t>
struct matrix_core {
  std::vector<value_t> data;
  std::pair<int, int> dimensions;
  decltype(auto) operator[](size_t i) { return data[i]; }
  matrix_core() = default;
  matrix_core(matrix_core<value_t>&) = default;
  matrix_core(matrix_core<value_t>&& other) {
    data = std::move(other.data);
    dimensions = std::move(other.dimensions);
  };
  matrix_core& operator=(matrix_core&& other) {
    data = std::move(other.data);
    dimensions = std::move(other.dimensions);
    return *this;
  }
  matrix_core& operator=(matrix_core&) = default;
  bool operator==(matrix_core<value_t>& other) {
    return dimensions == other.dimensions && data == other.data;
  }
};

// This namespace holds all the transformations we can do on the AST
namespace transform {
// This transformation returns the nth value from the core terminal node
struct at_index {
  template <typename T>
  decltype(auto) operator()(
      boost::yap::terminal<yap_matrix_expression, matrix_core<T>>& expr) {
    return boost::yap::make_terminal(boost::yap::value(expr).data[index]);
  }
  size_t index;
};
// This transformation counts the number of dot product nodes in the AST
struct count_dot_product_nodes {
  template <typename T>
  int operator()(boost::yap::expr_tag<boost::yap::expr_kind::terminal>,
                 matrix_core<T>& elem) {
    return 0;
  }
  template <typename lhs, typename rhs>
  int operator()(boost::yap::expr_tag<boost::yap::expr_kind::bitwise_or>,
                 lhs& lexpr, rhs& rexpr) {
    int left_side_count =
        boost::yap::transform(boost::yap::as_expr(lexpr), *this);
    int right_side_count =
        boost::yap::transform(boost::yap::as_expr(rexpr), *this);
    return left_side_count + right_side_count;
  }
};
// This transformation returns the dimension of the node. It is dynamically
// computed at runtime
struct get_node_dimension {
  template <typename T>
  // If a terminal is found return its dimension.
  std::pair<int, int> operator()(
      boost::yap::expr_tag<boost::yap::expr_kind::terminal>,
      matrix_core<T>& elem) {
    return boost::yap::value(elem).dimensions;
  }
  template <typename lhs, typename rhs>
  // If a plus node is found return operands dimensions recursing both sides.
  // Also assert on ambiguity
  std::pair<int, int> operator()(
      boost::yap::expr_tag<boost::yap::expr_kind::plus>, lhs& lexpr,
      rhs& rexpr) {
    auto left_dimension =
        boost::yap::transform(boost::yap::as_expr(lexpr), *this);
    auto right_dimension =
        boost::yap::transform(boost::yap::as_expr(rexpr), *this);
    assert(left_dimension == right_dimension);
    return left_dimension;
  }
  // Same for minus node
  template <typename lhs, typename rhs>
  std::pair<int, int> operator()(
      boost::yap::expr_tag<boost::yap::expr_kind::minus>, lhs& lexpr,
      rhs& rexpr) {
    auto left_dimension =
        boost::yap::transform(boost::yap::as_expr(lexpr), *this);
    auto right_dimension =
        boost::yap::transform(boost::yap::as_expr(rexpr), *this);
    assert(left_dimension == right_dimension);
    return left_dimension;
  }
  // Same for multiplies node
  template <typename lhs, typename rhs>
  std::pair<int, int> operator()(
      boost::yap::expr_tag<boost::yap::expr_kind::multiplies>, lhs& lexpr,
      rhs& rexpr) {
    auto left_dimension =
        boost::yap::transform(boost::yap::as_expr(lexpr), *this);
    auto right_dimension =
        boost::yap::transform(boost::yap::as_expr(rexpr), *this);
    assert(left_dimension == right_dimension);
    return left_dimension;
  }
  // | this operator is used for dot product in matrices
  template <typename lhs, typename rhs>
  std::pair<int, int> operator()(
      boost::yap::expr_tag<boost::yap::expr_kind::bitwise_or>, lhs& lexpr,
      rhs& rexpr) {
    auto left_dimension =
        boost::yap::transform(boost::yap::as_expr(lexpr), *this);
    auto right_dimension =
        boost::yap::transform(boost::yap::as_expr(lexpr), *this);
    assert(left_dimension.second == right_dimension.first);
    return {left_dimension.first, right_dimension.second};
  }
};
// Transform should not recurse and only should be called with
// boost::yap::transform_strict. Given a dot product node this transoform will
// return true if evaluating the dot product node eagerly is feasible. The
// factor that determines if we should evaluate the given dot expression is
// determined by feasibility_factor. In General it works like this :
// 	1. (A + B) | C
// 	2. (A +  B - C ) | (F - G * H)
// Expression 2 is more likely to return true because lazily evaluating this dot
// product expression will cause to evaluate more expressions both sides.
// However, Expression 1 can be lazily evaluate because one of its operands is
// already a terminal. The liklihood of evaluation is determined by
// feasibility_factor. Also in expressions containing nested dot product calls.
// It will be always evaluated eagerly after other optimizations have taken
// place. It may seem vague for a moment because eagerly evaluating such
// expression almost evaluates the complete expression, but in the Paper by
// Klaug for Smart Expression template, You will see that eagerly evaluated
// algorithms outperformed all lazy evaluating algorithms. In shorts, eagerly
// evaluating dot product is the best move
struct is_dot_eval_feasible {
  template <typename lhs, typename rhs>
  bool operator()(boost::yap::expr_tag<boost::yap::expr_kind::bitwise_or>,
                  lhs& lexpr, rhs& rexpr) {
    return true;
  };
  int feasibility_factor;
};

// This transform evaluates all the dot products as they are encountered and
// convertes it into a terminal This eager evaluation of the dot products must
// be done only when a transform named is_dot_eval_feasible returns true then
// only this evaluation should be done.
struct evaluate_intermediate_dot_products {
  template <typename lhs, typename rhs>
  decltype(auto) operator()(
      boost::yap::expr_tag<boost::yap::expr_kind::bitwise_or>, lhs& lexpr,
      rhs& rexpr) {
    return true;
  };
};

// This transform will modify the current node and possibly change the following
// nodes by using the distributive law to reduce the compute ops. Expressions
// such as A*B + A*C requires 3 ops (2 multiply and 1 add), however this can be
// optimized if we modify the expression like A * ( B + C ), it now envolves
// only 2 operations. This distributive law will work with + and -
struct optimize_with_distributive_law {
  template <typename lhs, typename rhs>
  // Distribution over addition
  decltype(auto) operator()(boost::yap::expr_tag<boost::yap::expr_kind::plus>,
                            lhs& lexpr, rhs& rexpr) {
    return true;
  };
  // Distribution over subtraction
  template <typename lhs, typename rhs>
  decltype(auto) operator()(boost::yap::expr_tag<boost::yap::expr_kind::minus>,
                            lhs& lexpr, rhs& rexpr) {
    return true;
  };
};

// This transform will convert an expression node below it into a terminal node
// always. It will use scalar law to optimize expressions such as A + B + A + B
// + A to 3*A + 2*B. This transform will always prune the AST by modifying the
// nodes.
struct optimize_with_scalar_law {};
// This transformation will also prune the AST very much by cancelling any or
// All expressions that can be cancelled easily. It is likely to convert an
// expression like A + B - (A + B + C + A) into C + A, cancelling all terms that
// are same and differ in sign. It can also cancel multiplication with divisions
// so an Expression like (A/B + C/B) * B becomes A + C after this transform
// takes place.
struct optimize_with_cancellation_law {};
}  // namespace transform

// Actual Matrix Expression starts here
template <boost::yap::expr_kind Kind, typename Tuple>
struct yap_matrix_expression {
  Tuple elements;
  details::device run_policy = Sequencial;
  const static boost::yap::expr_kind kind = Kind;
  decltype(auto) operator[](size_t i) const {
    return boost::yap::evaluate(
        boost::yap::transform(*this, transform::at_index{i}));
  }

  decltype(auto) via(details::device policy) {
    run_policy = policy;
    return *this;
  }
};
}  // namespace details

// Actual Matrix Starts here
template <class value_t>
class yap_matrix : public details::yap_matrix_expression<
                       boost::yap::expr_kind::terminal,
                       boost::hana::tuple<details::matrix_core<value_t>>> {
  decltype(auto) flatten_(std::vector<std::vector<value_t>>& data,
                          std::pair<int, int> dimen) {
    std::vector<value_t> tmp;
    for (int a = 0; a < dimen.first; a++)
      for (int b = 0; b < dimen.second; b++) tmp.push_back(data[a][b]);
    return tmp;
  }

  // This is a proxy marker that tells that the current variable contains a
  // evalauted or garbage value.
  bool has_been_evaluated = false;

 public:
  yap_matrix(std::vector<std::vector<value_t>> vecs) {
    details::matrix_core<value_t> core;

    has_been_evaluated = true;

    core.dimensions.first = vecs.size();
    core.dimensions.second = vecs[0].size();
    core.data = std::move(flatten_(vecs, core.dimensions));

    this->elements = boost::hana::tuple<decltype(core)>(std::move(core));
  }

  yap_matrix(yap_matrix&& rval) {
    has_been_evaluated = true;
    this->elements = std::move(rval.elements);
  }

  yap_matrix(yap_matrix& lval) {
    this->elements = lval.elements;
    has_been_evaluated = true;
  }

  template <typename Expr>
  yap_matrix(Expr&& expr) {
    details::matrix_core<value_t> core;
    auto dimen = boost::yap::transform(
        boost::yap::as_expr(expr), details::transform::get_node_dimension{});
    has_been_evaluated = true;
    core.data = std::move(std::vector<value_t>(dimen.first * dimen.second));

    if (expr.run_policy == details::Parallel) {
#pragma omp parallel for
      for (int a = 0; a < dimen.first * dimen.second; a++)
        core.data[a] = std::move(expr[a]);
    } else
      for (int a = 0; a < dimen.first * dimen.second; a++)
        core.data[a] = std::move(expr[a]);

    core.dimensions = std::move(dimen);
    this->elements = boost::hana::tuple<decltype(core)>(std::move(core));
  }

  template <typename Expr>
  yap_matrix& operator=(Expr&& expr) {
    auto dimen = boost::yap::transform(
        boost::yap::as_expr(expr), details::transform::get_node_dimension{});
    if (dimen != this->elements[0_c].dimensions) {
      // Warn or throw, the dimension that user is trying to assign to does not
      // match the dimension of variable. We can assign although, just by
      // creating a new core and std::move(new_core) to this->elements; But to
      // be on the safe side, only those expession can be assigned to matrix,
      // where both expression and variable dimensions match. So, We will throw
      // an exception here.
      throw std::logic_error(
          "Opps ! You tried to assign an expression to a matrix of different "
          "dimensions.");
    }
    if (expr.run_policy == details::Parallel) {
#pragma omp parallel for
      for (int a = 0; a < dimen.first * dimen.second; a++)
        this->elements[0_c].data[a] = std::move(expr[a]);
    } else {
      for (int a = 0; a < dimen.first * dimen.second; a++)
        this->elements[0_c].data[a] = std::move(expr[a]);
    }
    this->elements[0_c].dimensions = std::move(dimen);
    has_been_evaluated = true;
    return *this;
  }

  template <typename Expr>
  yap_matrix& operator=(Expr& expr) {
    auto dimen = boost::yap::transform(
        boost::yap::as_expr(expr), details::transform::get_node_dimension{});
    if (dimen != this->elements[0_c].dimensions) {
      throw std::logic_error(
          "Oppsiee. Assigning to this marix is not possible because "
          "expresssion has a different dimension");
    }
    if (expr.run_policy == details::Parallel) {
#pragma omp parallel for
      for (int a = 0; a < dimen.first * dimen.second; a++)
        this->elements[0_c].data[a] = expr[a];
    } else {
      for (int a = 0; a < dimen.first * dimen.second; a++)
        this->elements[0_c].data[a] = expr[a];
    }
    this->elements[0_c].dimensions = std::move(dimen);
    has_been_evaluated = true;
    return *this;
  }

  template <typename Expr>
  yap_matrix& operator+=(Expr&& expr) {
    if (!has_been_evaluated)
      throw std::logic_error(
          "This matrix must been evaluated before calling this expression");
    auto dimen = boost::yap::transform(
        boost::yap::as_expr(expr), details::transform::get_node_dimension{});
    if (dimen != this->elements[0_c].dimensions) {
      throw std::logic_error(
          "Oppisie, Matrix and Expression have different dimensions");
    }
    if (expr.run_policy == details::Parallel) {
#pragma omp parallel for
      for (int a = 0; a < dimen.first * dimen.second; a++)
        this->elements[0_c].data[a] += boost::yap::as_expr(expr)[a];
    } else {
      for (int a = 0; a < dimen.second * dimen.first; a++)
        this->elements[0_c].data[a] += boost::yap::as_expr(expr)[a];
    }
    return *this;
  }

  template <typename Expr>
  yap_matrix& operator-=(Expr&& expr) {
    if (!has_been_evaluated)
      throw std::logic_error(
          "This matrix must been evaluated before calling this expression");
    auto dimen = boost::yap::transform(
        boost::yap::as_expr(expr), details::transform::get_node_dimension{});
    if (dimen != this->elements[0_c].dimensions) {
      throw std::logic_error(
          "Oppisie, Matrix and Expression have different dimensions");
    }
    if (expr.run_policy == details::Parallel) {
#pragma omp parallel for
      for (int a = 0; a < dimen.first * dimen.second; a++)
        this->elements[0_c].data[a] += boost::yap::as_expr(expr)[a];
    } else {
      for (int a = 0; a < dimen.second * dimen.first; a++)
        this->elements[0_c].data[a] += boost::yap::as_expr(expr)[a];
    }
    return *this;
  }

  template <typename Expr>
  yap_matrix& operator*=(Expr&& expr) {
    if (!has_been_evaluated)
      throw std::logic_error(
          "This matrix must been evaluated before calling this expression");
    auto dimen = boost::yap::transform(
        boost::yap::as_expr(expr), details::transform::get_node_dimension{});
    if (dimen != this->elements[0_c].dimensions) {
      throw std::logic_error(
          "Oppisie, Matrix and Expression have different dimensions");
    }
    if (expr.run_policy == details::Parallel) {
#pragma omp parallel for
      for (int a = 0; a < dimen.first * dimen.second; a++)
        this->elements[0_c].data[a] += boost::yap::as_expr(expr)[a];
    } else {
      for (int a = 0; a < dimen.second * dimen.first; a++)
        this->elements[0_c].data[a] += boost::yap::as_expr(expr)[a];
    }
    return *this;
  }

  decltype(auto) get(int a, int b) {
    if (!has_been_evaluated) {
      throw std::logic_error("This matrix is not yet evaluated");
    }
    auto const& ref_dim = this->elements[0_c].dimensions;
    auto const& ref_data = this->elements[0_c].data;

    if (a >= ref_dim.first || b >= ref_dim.second)
      throw std::runtime_error("Invalid index for access");
    return ref_data[ref_dim.second * a + b];
  }

  template <typename Expr>
  bool operator==(Expr& expr) {
    auto dimen = boost::yap::transform(
        boost::yap::as_expr(expr), details::transform::get_node_dimension{});
    if (dimen.first != this->elements[0_c].dimensions.first ||
        dimen.second != this->elements[0_c].dimensions.second)
      return false;
    for (int a = 0; a < dimen.first * dimen.second; a++)
      if (this->elements[0_c].data[a] != expr[a]) return false;
    return true;
  }

  template <typename stream_v>
  void print(stream_v& cout) noexcept {
    auto const& ref_dim = this->elements[0_c].dimensions;
    for (int a = 0; a < ref_dim.first; a++) {
      for (int b = 0; b < ref_dim.second; b++) cout << this->get(a, b) << " ";
      cout << "\n";
    }
  }
};

// BOOST_YAP_USER_BINARY_OPERATOR(plus, details::yap_matrix_expression,
// details::yap_matrix_expression); // +
BOOST_YAP_USER_BINARY_OPERATOR(minus, details::yap_matrix_expression,
                               details::yap_matrix_expression);  // -
BOOST_YAP_USER_BINARY_OPERATOR(multiplies, details::yap_matrix_expression,
                               details::yap_matrix_expression);  // *

// Overload + without YAP MACROS
template <boost::yap::expr_kind K, typename A, boost::yap::expr_kind K2,
          typename B>
decltype(auto) operator+(details::yap_matrix_expression<K, A>& lexpr,
                         details::yap_matrix_expression<K2, B>& rexpr) {
  return boost::yap::make_expression<details::yap_matrix_expression,
                                     boost::yap::expr_kind::plus>(
      boost::yap::as_expr(lexpr), boost::yap::as_expr(rexpr));
}
#endif
