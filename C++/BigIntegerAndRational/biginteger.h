#include <iostream>
#include <vector>

class BigInteger {
 public:
  BigInteger() { sign_ = true; }

  BigInteger(const BigInteger& bigint)
      : body_(bigint.body_), sign_(bigint.sign_) {}

  void stringToBI(std::string& str_copy) {
    const int kBaseNumber = 9;
    const int kCharToInt = 48;
    const int kTen = 10;
    for (int64_t i = str_copy.length(); i > 0; i -= kBaseNumber) {
      if (i < kBaseNumber) {
        int64_t elem = 0;
        for (int64_t j = 0; j < i; ++j) {
          elem = elem * kTen + static_cast<int64_t>(str_copy[j]) - kCharToInt;
        }
        body_.push_back(elem);
      } else {
        int64_t elem = 0;
        for (int64_t j = i - kBaseNumber; j < i; ++j) {
          elem = elem * kTen + static_cast<int64_t>(str_copy[j]) - kCharToInt;
        }
        body_.push_back(elem);
      }
    }
    removeLeadingZeroes();
  }

  explicit BigInteger(const std::string& k_str) {
    std::string str_copy = k_str;
    if (str_copy.length() == 0) {
      sign_ = true;
    } else {
      if (str_copy[0] == '-') {
        str_copy = k_str.substr(1);
        sign_ = false;
      } else {
        sign_ = true;
      }

      stringToBI(str_copy);
    }
  }

  BigInteger(const int kElem) {
    int elem_copy = kElem;
    if (elem_copy < 0) {
      elem_copy *= -1;
      sign_ = false;
    } else {
      sign_ = true;
    }
    do {
      body_.push_back(elem_copy % kBase);
      elem_copy = elem_copy / kBase;
    } while (elem_copy != 0);
  }

  explicit BigInteger(const long long kElem) {
    long long elem_copy = kElem;
    if (elem_copy < 0) {
      elem_copy *= -1;
      sign_ = false;
    } else {
      sign_ = true;
    }
    do {
      body_.push_back(elem_copy % kBase);
      elem_copy = elem_copy / kBase;
    } while (elem_copy != 0);
  }

  static bool checkEqual(const BigInteger& left_copy,
                         const BigInteger& right_copy) {
    if ((left_copy.sign_ == right_copy.sign_) &&
        (left_copy.body_.size() == right_copy.body_.size())) {
      for (size_t i = 0; i < right_copy.body_.size(); ++i) {
        if (left_copy.body_[i] != right_copy.body_[i]) {
          break;
        }
        if (i == right_copy.body_.size() - 1) {
          return false;
        }
      }
    }

    return true;
  }

  bool operator<(const BigInteger& bigint) const {
    BigInteger left_copy = *this;
    BigInteger right_copy = bigint;
    if (!checkEqual(left_copy, right_copy)) {
      return checkEqual(left_copy, right_copy);
    }

    if (!left_copy.sign_) {
      if (!right_copy.sign_) {
        return ((-right_copy) < (-left_copy));
      }
      return true;
    }
    if (!right_copy.sign_) {
      return false;
    }
    if (left_copy.body_.size() != right_copy.body_.size()) {
      return left_copy.body_.size() < right_copy.body_.size();
    }
    for (int64_t i = static_cast<int64_t>(left_copy.body_.size()) - 1; i >= 0;
         --i) {
      if (left_copy.body_[i] != right_copy.body_[i]) {
        return left_copy.body_[i] < right_copy.body_[i];
      }
    }
    return false;
  }

  BigInteger& operator=(const BigInteger& bigint) {
    body_ = bigint.body_;
    sign_ = bigint.sign_;
    return *this;
  }

  explicit operator bool() const {
    return !((body_.empty()) || ((body_.size() == 1) && (body_[0] == 0)));
  }

  BigInteger operator-() const {
    BigInteger copy(*this);
    copy.sign_ = !copy.sign_;
    return copy;
  }

  BigInteger& operator+=(const BigInteger& value) {
    return *this = *this + value;
  }

  BigInteger& operator-=(const BigInteger& value) {
    return *this = (*this - value);
  }

  BigInteger& operator*=(const BigInteger& value) {
    return *this = (*this * value);
  }

  BigInteger& operator/=(const BigInteger& value) {
    return *this = (*this / value);
  }

  BigInteger& operator%=(const BigInteger& value) {
    return (*this = *this - (*this / value) * value);
  }

  void shiftRight() {
    if (body_.empty()) {
      body_.push_back(0);
      return;
    }
    body_.push_back(body_[body_.size() - 1]);
    for (size_t i = body_.size() - 2; i > 0; --i) {
      body_[i] = body_[i - 1];
    }
    body_[0] = 0;
  }

  std::string toString() const {
    std::string s;
    if (body_.empty()) {
      s = "0";
      return s;
    }
    const int kTen = 10;
    for (int64_t i = static_cast<int64_t>(body_.size()) - 1; i >= 0; --i) {
      if (i == static_cast<int64_t>(body_.size()) - 1) {
        s += std::to_string(body_[i]);
      } else {
        int elem;
        elem = body_[i];
        int j = kBase / kTen;
        while (j > 0) {
          s += std::to_string(elem / j);
          elem = elem % j;
          j = j / kTen;
        }
      }
    }
    if (!sign_) {
      if (s[0] != '0') {
        s = "-" + s;
      }
    }

    return s;
  }

  BigInteger& operator++() {
    *this += 1;
    return *this;
  }

  BigInteger operator++(int) {
    *this += 1;
    return (*this - 1);
  }

  BigInteger& operator--() {
    *this -= 1;
    return *this;
  }

  BigInteger operator--(int) {
    *this -= 1;
    return (*this + 1);
  }

  BigInteger operator*(const BigInteger& bigint) const {
    BigInteger result;
    result.body_.resize(body_.size() + bigint.body_.size());
    for (size_t i = 0; i < body_.size(); ++i) {
      int carry = 0;
      for (size_t j = 0; j < bigint.body_.size() || carry != 0; ++j) {
        int64_t cur =
            result.body_[i + j] +
                body_[i] * 1LL * (j < bigint.body_.size() ? bigint.body_[j] : 0) +
                carry;
        result.body_[i + j] = static_cast<int>(cur % BigInteger::kBase);
        carry = static_cast<int>(cur / BigInteger::kBase);
      }
    }
    result.sign_ = sign_ == bigint.sign_;
    result.removeLeadingZeroes();
    return result;
  }

  friend std::istream& operator>>(std::istream& in, BigInteger& bigint);

  friend BigInteger operator+(BigInteger left, const BigInteger& right);

  friend BigInteger operator-(BigInteger left, const BigInteger& right);

  friend BigInteger operator/(const BigInteger& left, const BigInteger& right);

 private:
  static const int32_t kBase = 1000000000;
  std::vector<int32_t> body_;
  bool sign_;

  void removeLeadingZeroes() {
    while (body_.size() > 1 && body_.back() == 0) {
      body_.pop_back();
    }
    if (body_.size() == 1 && body_[0] == 0) {
      sign_ = true;
    }
  }
};

std::ostream& operator<<(std::ostream& out, const BigInteger& bigint) {
  std::string str = bigint.toString();
  out << str;

  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& bigint) {
  std::string s;
  in >> s;
  BigInteger result(s);
  bigint = result;

  return in;
}

bool operator==(const BigInteger& left, const BigInteger& right) {
  return (!(left < right) && !(right < left));
}

bool operator!=(const BigInteger& left, const BigInteger& right) {
  return !(left == right);
}

bool operator<=(const BigInteger& left, const BigInteger& right) {
  return (left < right || left == right);
}

bool operator>(const BigInteger& left, const BigInteger& right) {
  return !(left <= right);
}

bool operator>=(const BigInteger& left, const BigInteger& right) {
  return !(left < right);
}

BigInteger operator+(BigInteger left, const BigInteger& right) {
  if (!left.sign_) {
    if (!right.sign_) {
      return -(-left + (-right));
    }
    return right - (-left);
  }
  if (!right.sign_) {
    return left - (-right);
  }

  int carry = 0;
  for (size_t i = 0;
       i < std::max(left.body_.size(), right.body_.size()) || carry != 0; ++i) {
    if (i == left.body_.size()) {
      left.body_.push_back(0);
    }
    left.body_[i] += carry + (i < right.body_.size() ? right.body_[i] : 0);
    carry = static_cast<int>(left.body_[i] >= BigInteger::kBase);
    if (carry != 0) {
      left.body_[i] -= BigInteger::kBase;
    }
  }

  return left;
}

BigInteger operator-(BigInteger left, const BigInteger& right) {
  if (!right.sign_) {
    return left + (-right);
  }
  if (!left.sign_) {
    return -((-left) + right);
  }
  if (left < right) {
    return -(right - left);
  }

  int carry = 0;
  for (size_t i = 0; i < right.body_.size() || carry != 0; ++i) {
    left.body_[i] -= carry + (i < right.body_.size() ? right.body_[i] : 0);
    carry = static_cast<int>(left.body_[i] < 0);
    if (carry != 0) {
      left.body_[i] += BigInteger::kBase;
    }
  }
  left.removeLeadingZeroes();
  return left;
}

BigInteger operator/(const BigInteger& left, const BigInteger& right) {
  BigInteger b = right;
  b.sign_ = true;
  BigInteger result;
  BigInteger current;
  result.body_.resize(left.body_.size());
  for (int64_t i = static_cast<int64_t>(left.body_.size()) - 1; i >= 0; --i) {
    current.shiftRight();
    current.body_[0] = left.body_[i];
    current.removeLeadingZeroes();
    int x = 0;
    int l = 0;
    int r = BigInteger::kBase;
    while (l <= r) {
      int m = (l + r) / 2;
      BigInteger t = b * m;
      if (t <= current) {
        x = m;
        l = m + 1;
      } else {
        r = m - 1;
      }
    }
    result.body_[i] = x;
    current = current - b * x;
  }
  result.sign_ = left.sign_ == right.sign_;
  result.removeLeadingZeroes();
  return result;
}

BigInteger operator%(const BigInteger& left, const BigInteger& right) {
  BigInteger result(left);
  result %= right;
  return result;
}

BigInteger Gcd(const BigInteger& left, const BigInteger& right) {
  BigInteger left_copy(left);
  BigInteger right_copy(right);
  if (left_copy < 0) {
    left_copy *= -1;
  }
  if (right_copy < 0) {
    right_copy *= -1;
  }
  while (right_copy != 0) {
    left_copy = left_copy % right_copy;
    BigInteger c = left_copy;
    left_copy = right_copy;
    right_copy = c;
  }

  return left_copy;
}

class Rational {
 public:
  Rational() {
    numerator_ = 0;
    denominator_ = 1;
    setSign();
  }

  Rational(int elem) {
    numerator_ = elem;
    denominator_ = 1;
    setSign();
  }

  Rational(const BigInteger& bigint) {
    numerator_ = bigint;
    denominator_ = 1;
    setSign();
  }

  Rational(const Rational& ration) {
    numerator_ = ration.numerator_;
    denominator_ = ration.denominator_;
    setSign();
  }

  std::string toString() const {
    std::string s;
    s += numerator_.toString();
    if (denominator_ != 1) {
      s += '/' + denominator_.toString();
    }
    return s;
  }

  std::string asDecimal(size_t precision) const {
    Rational copy(*this);
    BigInteger result = copy.numerator_ / copy.denominator_;
    std::string s;
    s = result.toString();
    if (s == "0") {
      if (copy < 0) {
        s = "-" + s;
      }
    }
    s += ".";
    copy -= result;
    if (copy < 0) {
      copy *= -1;
    }
    BigInteger num = copy.numerator_;
    BigInteger den = copy.denominator_;

    const int kTen = 10;
    for (size_t i = 0; i < precision; ++i) {
      num *= kTen;
      if (num < den) {
        s += "0";
      }
    }
    result = num / den;
    s += result.toString();

    return s;
  }

  bool operator==(const Rational& ration) const {
    return (numerator_ == ration.numerator_) &&
        (denominator_ == ration.denominator_);
  }

  bool operator<(const Rational& ration) const {
    if (*this == ration) {
      return false;
    }

    Rational copy(*this);
    Rational ration_copy(ration);
    BigInteger gcd = Gcd(copy.denominator_, ration_copy.denominator_);
    copy *= ration_copy.denominator_ / gcd;
    ration_copy *= copy.denominator_ / gcd;
    return copy.numerator_ < ration_copy.numerator_;
  }

  Rational& operator+=(const Rational& value) {
    BigInteger gcd = Gcd(denominator_, value.denominator_);
    BigInteger dop = value.numerator_ * denominator_;
    denominator_ *= value.denominator_ / gcd;
    numerator_ *= value.denominator_ / gcd;
    numerator_ += dop / gcd;
    setSign();
    gcd = Gcd(numerator_, denominator_);
    numerator_ /= gcd;
    denominator_ /= gcd;
    return *this;
  }

  Rational& operator-=(const Rational& value) {
    *this += -value;
    return *this;
  }

  Rational& operator*=(const Rational& value) {
    numerator_ *= value.numerator_;
    denominator_ *= value.denominator_;
    setSign();
    BigInteger gcd = Gcd(numerator_, denominator_);
    numerator_ /= gcd;
    denominator_ /= gcd;
    return *this;
  }

  Rational& operator/=(const Rational& value) {
    numerator_ *= value.denominator_;
    denominator_ *= value.numerator_;
    setSign();
    BigInteger gcd = Gcd(numerator_, denominator_);
    numerator_ /= gcd;
    denominator_ /= gcd;
    return *this;
  }

  Rational operator-() const {
    Rational copy(*this);
    copy *= -1;
    return copy;
  }

 private:
  BigInteger numerator_;
  BigInteger denominator_;

  void setSign() {
    if (denominator_ < 0) {
      denominator_ *= -1;
      numerator_ *= -1;
    }
  }
};

std::ostream& operator<<(std::ostream& out, const Rational& ration) {
  std::string str = ration.toString();
  out << str;
  return out;
}

bool operator!=(const Rational& left, const Rational& right) {
  return !(left == right);
}

bool operator>(const Rational& left, const Rational& right) {
  return !(left == right || left < right);
}

bool operator<=(const Rational& left, const Rational& right) {
  return left == right || left < right;
}

bool operator>=(const Rational& left, const Rational& right) {
  return (left == right || left > right);
}

Rational operator+(const Rational& left, const Rational& right) {
  Rational result(left);
  result += right;
  return result;
}

Rational operator-(const Rational& left, const Rational& right) {
  Rational result(left);
  result -= right;
  return result;
}

Rational operator*(const Rational& left, const Rational& right) {
  Rational result(left);
  result *= right;
  return result;
}

Rational operator/(const Rational& left, const Rational& right) {
  Rational result(left);
  result /= right;
  return result;
}
