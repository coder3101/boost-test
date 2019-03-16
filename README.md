# YAP Matrix : A Programming competency test for uBLAS

[![Build 
Status](https://travis-ci.org/coder3101/gsoc19-boost-test.svg?branch=with-yap)](https://travis-ci.org/coder3101/gsoc19-boost-test)

This repository contains the programming competency test code for `boost.uBLAS` Linear Algebra Proposal of **Google Summer Of Code 2019**. It uses `boost.YAP` to achieve smart expression templates.

#### Beware other my friends (Students)

> *Do not try to copy this code to apply into `boost.uBLAS` this summer 2019 at Google Summer of Code. All the code in this repository is already submitted by me. Although You are free to take some idea from this repository*

---

## Installation

Run 

```bash
cd build && cmake .. && sudo make install
```

It will install the library to `/usr/local/include`.

Now you should be able to compile following, 

```cpp

#include <yap_matrix.hpp>

int main(){
    using test::yap;
    matrix a(std::vector(20, 2), 5, 4); // 5 x 4 Matrix with 2 filled in all cells.
    matrix b(std::vector(20, 0), 5, 4); // Same as above but 0 filled in all cells.
    auto expression = ((a * a * a) + (a / 5)); // All operations are element-wise.
    assign(b, expression); // Evaluates the expression and put into b.
    return 0;
}

```

## Uninstallation

It's simple, simply remove the file from the directory `/usr/local/include/` by running :

```bash

sudo rm /usr/local/include/yap_matrix.hpp

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



> | Operation | Descriptions                                                 | Supported Operands type |
> | --------- | ------------------------------------------------------------ | ----------------------- |
> | +         | Adds element-wise to the left hand side expression           | Matrix or Scalar        |
> | -         | Subtracts elements-wise to the left hand side expression     | Matrix or Scalar        |
> | *         | Multiplies element-wise to the left hand side expression     | Matrix or Scalar        |
> | /         | Divides element-wise to the left hand side expression        | Matrix or Scalar        |
> | %         | Modulus element-wise to the left hand side expression        | Matrix or Scalar        |
> | - (unary) | Flips the sign of each element                               | Matrix                  |
> | <         | Computes if left hand side is less than right hand side      | Matrix or Scalar        |
> | >         | Computes if left hand side is more than right hand side      | Matrix or Scalar        |
> | `>=`      | Computes if left hand side is more or equal to right hand side | Matrix or Scalar        |
> | <=        | Computes if left hand side is less or equal to right hand side | Matrix or Scalar        |
> | ==        | Checks if two Matrices are equal                             | Matrix                  |
> | !=        | Checks if two Matrices are not equal                         | Matrix                  |
> | &&        | Logical Or of two sides. They must be convertible to bool    | Matrix or Scalar        |
> | \|\|      | Logical And of two sides. They must be convertible to bool   | Matrix or Scalar        |
> | ^         | Element wise XOR of two sides.                               | Matrix or Scalar        |
> | &         | Element wise bit-wise AND of two sides                       | Matrix or Scalar        |
> | \|        | Element wise bit-wise OR of two sides                        | Matrix or Scalar        |

## Builds

I used CMake as the Build-tool-generator. Main's artifact could be found at `./build` folder named `main` . 

**The complete code has been formatted using `clang-format` using `Visual Studio` coding style.**



## Issues and Improvements

**While I know No Code is Perfect and that there is always room for improvements. I welcome all your suggestions and feedback on this test.**

---

# My Projects

I have an Open-Source Library named `testcaser` (still in beta) that contains more than 10,000 lines of code please check it at https://github.com/coder3101/testcaser .  You can also check other projects.

---



*Wishing for a A Happy News, this Summer*
