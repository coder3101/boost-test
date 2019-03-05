# Matrix : A Programming competency test for uBLAS

[![Build 
Status](https://travis-ci.org/coder3101/gsoc19-boost-test.svg?branch=master)](https://travis-ci.org/coder3101/gsoc19-boost-test)

This repository contains the programming competency test code for `boost.uBLAS` Linear Algebra Proposal of **Google Summer Of Code 2019**. 

#### Beware other my friends (Students)

> *Do not try to copy this code to apply into `boost.uBLAS` this summer 2019 at Google Summer of Code. All the code in this repository is already submitted by me. Although You are free to take some idea from this repository*

---

## Installation

Run 

```bash
cd build && cmake .. && make install
```

It will install the library to `/usr/local/include` and you can use to as a normal header from there after.

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

### Creating a Matrix

All Matrix variable have immutable dimensions. i.e It cannot be changed once set. We have following 3 constructor of Matrix. You need to pass an optional one of the two policies `test::policy::RowMajorPolicy` or `test::policy::ColumnMajorPolicy` which determines the policy for storing the elements in flat order. The Policy order defaults to `RowMajorPolicy`

- Using Initializer List of Initializer Lists

  - ```cpp
    using test;
    matrix<int,policy::ColumnMajorPolicy> foo = {{1,2,3},
                                         	     {4,5,6},
                                                 {7,8,9}};
    ```

- Using Vector of Vectors

- Using just row and column count.

  - ```cpp
    test::matrix<int> baaz(rows, columns);
    ```



The `test::matrix` type can be converted to and from `test::expression` type. An Expression represents the operation to be computed. We have a non-explicit constructor that takes in a `expression` and evaluates it to form the `test::matrix` .  An expression type will be evaluated upon the call to any assignment operator (=, +=, -= ...etc).



### Operations on Matrix

This is a simple library, it does not have a lot to offer when it comes to operations. But it does have almost all basic operations using operator overloading. Following is a list of all the supported operations.

|  Operation   | Description of Operation                                     | Is Lazy? |
| :----------: | ------------------------------------------------------------ | -------- |
|  operator=   | Assign the value to *this* variable. If LHS is an expression it is evaluated and result is stored into *this* | No       |
|  operator+=  | Element wise Add and assign to *this*. If LHS is an expression it is evaluated and result is stored into *this* | No       |
|  operator-=  | Element wise Subtract and assign to *this*. If LHS is an expression it is evaluated and result is stored into *this* | No       |
|  operator*=  | Element wise Multiplication and assign to *this*. If LHS is an expression it is evaluated and result is stored into *this* | No       |
|  operator/=  | Element wise Division and assign to *this*. If LHS is an expression it is evaluated and result is stored into *this* | No       |
|  operator==  | Element wise Equality check. If all elements are same then true otherwise false is returned. If LHS is an expression it is evaluated then compared. | No       |
| scalar_add() | Adds the scalar argument to the matrix. It is evaluated eagerly | No       |
| scalar_sub() | Subtracts the scalar argument from the matrix. It is evaluated eagerly | No       |
| scalar_mul() | Multiplies the scalar argument to the matrix. It is evaluated eagerly | No       |
|    view()    | Shows the content of the matrix into stdout or another std::ostream | N/A      |
|  operator+   | Adds two same dimension matrices together element-wise.      | Yes      |
|  operator-   | Adds two same dimension matrices together element-wise.      | Yes      |
|  operator*   | Multiplies two same dimension matrices together element-wise. | Yes      |
|  operator/   | Divides two same dimension matrices together element-wise.   | Yes      |
|  operator\|  | Computes dot product of two matrices. It is eagerly evaluated | No       |



### Performance

Performance is the ultimate goal at the end of the day. Below is an image demonstrating the performance of `matrix` over the normal implementation. The benchmark was computed after running 192 length long +, -, *, / operations on 1000x1000 matrix.

**It is at-least 86% faster than naive implementation** Of-course, it can be further improved.

![Imgur](https://i.imgur.com/1Lrv8W4.png)



## Documentation

The complete Documentation is available at https://coder3101.github.io/gsoc19-boost-test 

The Documentation was automatically generated using `doxygen` from the source code.

## Builds

I used CMake as the Build-tool-generator. Main's artifact could be found at `./build` folder named `main` . 

**The complete code has been formatted using `clang-format` using `LLVM` coding style.**



## Issues and Improvements

**While I know No Code is Perfect and that there is always room for improvements. I welcome all your suggestions and feedback on this test.**

---

# My Projects

I have an Open-Source Library named `testcaser` (still in beta) that contains more than 10,000 lines of code please check it at https://github.com/coder3101/testcaser .  You can also check other projects.

---



*Wishing for a A Happy News, this Summer*
