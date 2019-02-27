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

#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <functional>
#include <iostream>

#ifndef BEAUTIFICATION_FACTOR
#define BEAUTIFICATION_FACTOR (80)
#endif

namespace boost::test {
struct benchmark_result {
  long long execution_time;
  std::string label;
  benchmark_result() = delete;
  benchmark_result(long long e, std::string &lbl)
      : execution_time(e), label(lbl){};

  void print_beautifully() {
    for (int t = 0; t < BEAUTIFICATION_FACTOR; t++) std::cout << "*";

    std::cout << "\n"
              << label << " executed for " << execution_time << " miliseconds ("
              << execution_time / 1000.0 << " s) \n";

    for (int t = 0; t < BEAUTIFICATION_FACTOR; t++) std::cout << "*";
    std::cout << std::endl;
  }
};

struct benchmark {
  static auto run(std::string label,std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto stop = std::chrono::high_resolution_clock::now();
    return benchmark_result(
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start)
            .count(),
        label);
  }
};

}  // namespace boost::test
#endif