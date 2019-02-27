# Programming competency test : uBLAS

This repository contains the test code for uBLAS Linear Algebra Proposal of **Google Summer Of Code 2019**. 



The code contains following things :

- Generic Matrix Class(es)

  - It contains two types of Matrix Class. One is `Matrix` and other called `blas_matrix`. `Matrix` contains all operations using traditional operator overloads. It is only meant for benchmark purpose to show how fast **`blas_matrix` is which uses expression templates for `lazy evalution`**.

- Contains Following operator Overloads and Special Functions

  | Operator / Functions available  | Normal Matrix (normal_matrix.hpp) | BLAS Matrix (blas_matrix.hpp) |
  | :-----------------------------: | --------------------------------- | ----------------------------- |
  |          + (Addition)           | YES                               | YES (with lazy eval)          |
  |        -  (Subtraction)         | YES                               | YES (with lazy eval)          |
  | * (Element-wise Multiplication) | YES                               | YES (with lazy eval)          |
  |    / (Element-wise Division)    | YES                               | YES (with lazy eval)          |
  |          == (Equality)          | YES                               | YES (with lazy eval)          |
  |               +=                | NO                                | YES (with lazy eval)          |
  |               -=                | NO                                | YES (with lazy eval)          |
  |               *=                | NO                                | YES (with lazy eval)          |
  |               /=                | NO                                | YES (with lazy eval)          |
  |         scalar_add(T t)         | NO                                | YES (with lazy eval)          |
  |         scalar_sub(T t)         | NO                                | YES (with lazy eval)          |
  |         scalar_mul(T t)         | NO                                | YES (with lazy eval)          |
  |       view (Show Matrix)        | YES                               | YES                           |
  |   \| (Dot Product of Matrix)    | NO                                | YES (with lazy eval)          |

- `benchmark.hpp` contains a method (run) for bench-marking the execution time of a function. We use this to compare how fast out BLAS Matrix is as compared to Normal Matrix. It then also prints the result in a beautiful way in the console (with ***)

- `result.png` contains a screenshot of the result of execution of the two matrix classes. The bench-mark ran 192 length long single matrix expression with 1000 by 1000 elements. The Operation performed was Element wise-addition. The results are as follow :

  ![Imgur](https://i.imgur.com/yWJl6rb.png)

* `main.cc` contains the driver program.
* I have tried my best to use all the modern C++11 and C++14/17 features. 



## Builds

I used CMake as the Build-tool-generator. Main's artifact could be found at `./build` folder named `main`



## Issues and Improvements

**While I know No Code is Perfect and that there is always room for improvements. I welcome all your suggestions and feedback on this test.**

---

# My Projects

I have an OpenSource Library named `testcaser` (still in beta) that contains more than 10,000 lines of code please check it at https://github.com/coder3101/testcaser .  You can also check other projects.

---



*Wishing for a A Happy News, this Summer*