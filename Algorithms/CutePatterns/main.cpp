#include <iostream>
#include <vector>
 
int64_t mod;
 
bool isOk(int64_t mask1, int64_t mask2, int64_t deg) {
  for (int64_t i = 1; i < deg; ++i) {
    if ((((mask1 >> i)&1) == ((mask2 >> i)&1)) &&
        (((mask1 >> i)&1) == ((mask2 >> (i - 1))&1)) &&
        (((mask1 >> i)&1) == ((mask1 >> (i -1))&1))) {
      return false;
    }
  }
  return true;
}
 
class Matrix {
 public:
  std::vector<std::vector<int64_t>> body_;
  int64_t a_;
  int64_t s_;
 
  Matrix(int64_t n, int64_t a) {
    s_ = n;
    a_ = a;
    for (int64_t i = 0; i < n; ++i) {
      body_.emplace_back();
      for (int64_t j = 0; j < n; ++j) {
        if (isOk(i, j, a_)) {
          body_[i].push_back(1);
        } else {
          body_[i].push_back(0);
        }
      }
    }
  }
 
  int64_t getSum() {
    int64_t res = 0;
    for (int64_t i = 0; i < s_; ++i) {
      for (int64_t j = 0; j < s_; ++j) {
        res += body_[i][j];
      }
    }
    return res % mod;
  }
 private:
};
 
Matrix operator*(const Matrix& left, const Matrix& right) {
  Matrix result(left.s_, left.a_);
  for (int64_t i = 0; i < left.s_; ++i) {
    for (int64_t j = 0; j < left.s_; ++j) {
      result.body_[i][j] = 0;
      for (int64_t k = 0; k < left.s_; ++k) {
        result.body_[i][j] += left.body_[i][k] * right.body_[k][j];
      }
      result.body_[i][j] %= mod;
    }
  }
 
  return result;
}
 
int64_t getDegree(int64_t n) {
  int64_t res = 1;
  res = res << n;
  return res;
}
 
int main() {
  int64_t t;
  std::cin >> t;
  for (int64_t i = 0; i < t; ++i) {
    int64_t a;
    int64_t m;
    std::cin >> m;
    std::cin >> a;
    int64_t modul;
    std::cin >> modul;
    mod = modul;
    int64_t n = getDegree(a);
    Matrix carry(n, a);
    Matrix res(n, a);
 
    if (m == 1) {
      std::cout << getDegree(a) % mod << "\n";
    } else {
      m -= 2;
      while (m != 0) {
        if (m % 2 == 0) {
          carry = carry * carry;
          m /= 2;
        } else {
          res = res * carry;
          m -= 1;
        }
      }
      std::cout << res.getSum() << "\n";
    }
  }
}
