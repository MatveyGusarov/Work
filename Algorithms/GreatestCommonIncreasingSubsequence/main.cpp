#include <iostream>
#include <vector>
 
int main() {
  int n;
  int m;
  
  std::cin >> n;
  std::cin >> m;
 
  std::vector<int> a;
  std::vector<int> b;
  for (int i = 0; i < n; ++i) {
    int elem;
    std::cin >> elem;
    a.push_back(elem);
  }
 
  for (int i = 0; i < m; ++i) {
    int elem;
    std::cin >> elem;
    b.push_back(elem);
  }
 
  std::vector<std::vector<int>> dp;
  for (int i = 0; i < n + 1; ++i) {
    dp.emplace_back();
    for (int j = 0; j < m + 1; ++j) {
      dp[i].push_back(0);
    }
  }
 
  for (int i = 1; i < n + 1; ++i) {
    int best = 0;
    for (int j = 1; j < m + 1; ++j) {
      dp[i][j] = dp[i - 1][j];
      if ((a[i - 1] == b[j - 1]) && (dp[i - 1][j] < best + 1)) {
        dp[i][j] = best + 1;
      }
      if ((a[i - 1] > b[j - 1]) && (dp[i - 1][j] > best)) {
        best = dp[i - 1][j];
      }
    }
  }
 
  int ans = 0;
  for (int i = 0; i < n + 1; ++i) {
    for (int j = 0; j < m + 1; ++j) {
      ans = std::max(ans, dp[i][j]);
    }
  }
 
  std::cout << ans << "\n";
}
