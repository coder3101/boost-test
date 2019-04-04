#include <boost/yap/print.hpp>
#include <boost/yap/yap.hpp>
#include <iostream>
#include <yap_matrix.hpp>

int main() {
  std::vector<std::vector<int>> elem, elem2;
  
  for (auto e : {0, 0, 0, 0, 0}) {  // Just for iterating 5 times
    elem.push_back({1, 2, 3});
    elem2.push_back({4, 5, 6});
  }

  yap_matrix<int> mat1(elem);   // This matrix is 5x3
  yap_matrix<int> mat2(elem2);  // This matrix is 5x3 also
  
  std::cout<<"Matrix A: \n";
  mat1.print(std::cout);
  std::cout<<"Matrix B: \n";
  mat2.print(std::cout);

  std::cout<<"Making Expression : ((A + B) * (B - A)) * A \n";
  
  auto expr = ((mat1 + mat2) * (mat2 - mat1)) * mat1;
  
  std::cout<<"YAP Abstrat syntax tree for expression\n";
  boost::yap::print(std::cout, expr);
  
  std::cout<<"Creating the result with move constructor for expr\n";
  yap_matrix<int> a((mat1 + mat2) * (mat2 - mat1) * mat1);
  
  std::cout<<"Creating the result with copy constructor for expr\n";
  yap_matrix<int> b(expr);

  std::cout<<"Creating the result with assignment of expression\n";
  yap_matrix<int> v(mat1);
  v = expr;

  // Execute this expression with Parallel thread
  std::cout<<"Creating a expression with assignment evaluated via parallel device\n";
  yap_matrix<int> w(mat1);
  w = ((mat1 + mat2) * (mat2 - mat1) * mat1).via(details::Parallel);
  

  //assert(w == v);
  //assert(v == b);
  //assert(b == a);
  assert(a == expr);

  std::cout<<"Evaluated Expression Result Matrix \n";
  a.print(std::cout);

  auto res = boost::yap::transform(expr, details::transform::get_node_dimension{});
  std::cout<<"Expression was Dimension : ["<<res.first<<" , "<<res.second << "]\n"; 
  
  a += (mat2 - mat1);
  a += expr;

  return 0;
}
