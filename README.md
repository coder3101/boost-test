# YAP Matrix : A Programming competency test for uBLAS

[![Build 
Status](https://travis-ci.org/coder3101/gsoc19-boost-test.svg?branch=with-yap)](https://travis-ci.org/coder3101/gsoc19-boost-test)

This repository contains the programming competency test code for `boost.uBLAS` Linear Algebra Proposal of **Google Summer Of Code 2019**. It uses `boost.YAP` to achieve smart expression templates.

**YOU MUST HAVE BOOST INSTALLED TO COMPILE  THIS PROJECT USING CMAKE*

#### Beware other my friends (Students)

> *Do not try to copy this code to apply into `boost.uBLAS` this summer 2019 at Google Summer of Code. All the code in this repository is already submitted by me. Although You are free to take some idea from this repository*

---

## Installation

Run 

```bash
cd build && cmake .. && sudo make install
```

It will install the library to `/usr/local/include`.

## Uninstallation

It's simple, simply remove the file from the directory `/usr/local/include/` by running :

```bash
sudo rm /usr/local/include/yap_matrix.hpp
```

### Example
```cpp
#include <boost/yap/print.hpp>
#include <boost/yap/yap.hpp>
#include <iostream>
#include "yap_matrix.hpp"

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

  yap_matrix<int> w(mat1);
  w = ((mat1 + mat2) * (mat2 - mat1) * mat1);
  

  assert(a == expr);

  std::cout<<"Evaluated Expression Result Matrix \n";
  a.print(std::cout);

  auto res = boost::yap::transform(expr, details::transform::get_node_dimension{});
  std::cout<<"Expression was Dimension : ["<<res.first<<","<<res.second << "]\n"; 
  
  a += (mat2 - mat1);
  a += expr;

  return 0;
}


```
### Output
```
Matrix A: 
1 2 3 
1 2 3 
1 2 3 
1 2 3 
1 2 3 
Matrix B: 
4 5 6 
4 5 6 
4 5 6 
4 5 6 
4 5 6 
Making Expression : ((A + B) * (B - A)) * A 
YAP Abstrat syntax tree for expression
expr<*>
    expr<*>
        expr<+>
            term<details::matrix_core<int>>[=<<unprintable-value>>] &
            term<details::matrix_core<int>>[=<<unprintable-value>>] &
        expr<->
            term<details::matrix_core<int>>[=<<unprintable-value>>] &
            term<details::matrix_core<int>>[=<<unprintable-value>>] &
    term<details::matrix_core<int>>[=<<unprintable-value>>] &
Creating the result with move constructor for expr
Creating the result with copy constructor for expr
Creating the result with assignment of expression
Evaluated Expression Result Matrix 
15 42 81 
15 42 81 
15 42 81 
15 42 81 
15 42 81 
Expression was Dimension : [5 , 3]
```

## Introduction to Matrix

Matrix is a Single Header only Library that includes some basic subroutines for Matrix Calculations. It is fast thanks to expression templates. It can expand expression like :

```cpp
auto val = a + b - c * d;
```

into

```cpp
for(size_t i=0; i< rows; i++)
    for(size_t j=0; j<cols; j++)
        vals[i][j] = a[i][j] + b[i][j] - c[i][j] * d[i][j];
```

Removing the time of copying on every operation. It uses expression template to achieve this task. It also means that any expression that does not calls `=` (assignment) the expression is not at all evaluated. 

### Operations on Matrix

This is a simple library, it does not have a lot to offer when it comes to operations. But it does have almost all basic operations using operator overloading. Following is a list of all the supported operations. All are element-wise operations.



|      Operator| Name |      Description|
| ---- | ---- | ---- |
| + | Element-wise Addition | Adds two expressions and adds a new node in AST for the operation |
| - | Element-wise Subtraction | Subtracts two expression or matrix and adds the operation node on AST |
| * | Element-wise Multiplication | Multiplies two expression or matrix and adds the operation to AST |
| += | Add and assign | LHS must be a Matrix and RHS can be expression or matrix. |
| -= | Subtract and assign | LHS must be a Matrix and RHS can be expression or matrix. |
| *= | Multiply and assign | LHS must be a Matrix and RHS can be expression or matrix. |
| \| | Dot product | *This is not implemented, Just for AST Dimension deduction it is there* |




## Builds

I used CMake as the Build-tool-generator. Main's artifact could be found at `./build` folder named `example` . 


## Issues and Improvements

**While I know No Code is Perfect and that there is always room for improvements. I welcome all your suggestions and feedback on this test.**

---

# My Projects

I have an Open-Source Library named `testcaser` (still in beta) that contains more than 10,000 lines of code please check it at https://github.com/coder3101/testcaser .  You can also check other projects.

---



*Wishing for a A Happy News, this Summer*
