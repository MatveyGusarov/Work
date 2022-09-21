#include <iostream>
#include <vector>
 
void Dynamic(int64_t power_multitude,
             std::vector<std::vector<int64_t>>& power_on_prefix) {
  for (int64_t power = 0; power < power_multitude; ++power) {
    for (int64_t prefix = 0; prefix < power_multitude; ++prefix) {
      if (prefix > 0) {
        power_on_prefix[power][prefix] += power_on_prefix[power][prefix - 1];
        if ((power - prefix > 0) && (prefix / 2 >= 0)) {
          power_on_prefix[power][prefix] +=
              power_on_prefix[power - prefix - 1][(prefix - 1) / 2];
        } else if (power == prefix) {
          power_on_prefix[power][prefix] += 1;
        }
      } else {
        if (power != 0) {
          power_on_prefix[power][prefix] = 0;
        }
      }
    }
  }
}
 
int main() {
  int64_t power_multitude;
  std::cin >> power_multitude;
  std::vector<std::vector<int64_t>> power_on_prefix;
  for (int64_t i = 0; i < power_multitude; ++i) {
    power_on_prefix.emplace_back();
    for (int64_t j = 0; j < power_multitude; ++j) {
      power_on_prefix[i].push_back(0);
    }
  }
  power_on_prefix[0][0] = 1;
 
  Dynamic(power_multitude, power_on_prefix);
  std::cout << power_on_prefix[power_multitude - 1][power_multitude - 1]
            << "\n";
}
