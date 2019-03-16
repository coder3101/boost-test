#include <include/yap_matrix.hpp>
#include <iostream>

template <typename value_t>
void view(test::yap::matrix<value_t> const& ref) {
  std::cout << "Viewing a new YAP Matrix\n";
  auto const& prop = ref.get_properties();
  for (int t = 0; t < prop.rows; t++) {
    for (int j = 0; j < prop.cols; j++) std::cout << ref.at(t, j) << " ";
    std::cout << "\n";
  }
}

int main() {
  using namespace test::yap;

  matrix a(std::vector<int>(20, 1), 5, 4);
  matrix b(std::vector<int>(20, 2), 5, 4);
  matrix c(std::vector<int>(20, 3), 5, 4);
  matrix d(std::vector<int>(20, 0), 5, 4);
  matrix e(std::vector<double>(20, 0.0), 5, 4);

  view(a);

  assign(a, 2);          // Assigns 2 to every element in the matrix.
  assign(a, b * 3);      // Assigns 3 times b to a element-wise.
  assign(b, a + b * c);  // Performs element wise operations and assigns to b
  view(b);
  assign(e, c);
  e += (e - 4) / (c % 1);  // All Operators are overloaded with expressions. All
                           // are element-wise operations.
  view(e);
  return 0;
}