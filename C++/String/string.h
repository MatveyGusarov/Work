#include <iostream>
#include <cstring>

class String {
 public:
  String() : my_string(new char[10]), size_(0), capacity_(10) {}

  String(const char* _string) : my_string(new char[2 * strlen(_string)]), size_(strlen(_string)),
                                capacity_(2 * strlen(_string)) {
    memcpy(my_string, _string, size_);
  }

  String(size_t n, char c) : my_string(new char[2 * n]), size_(n), capacity_(2 * n) {
    memset(my_string, c, n);
  }

  String(const String& str) {
    copy_method(str);
  }

  String& operator=(const String& str) {
    if (this != &str) {
      clear();
      copy_method(str);
    }
    return *this;
  }

  ~String() {
    delete[] my_string;
  }

  bool operator==(const String& str) {
    if (str.length() == size_) {
      for (size_t i = 0; i < size_; ++i) {
        if (str[i] != my_string[i])
          return false;
      }
    }
    return true;
  }

  String& operator+=(char sym) {
    push_back(sym);
    return *this;
  }

  String& operator+=(const String& str) {
    while (size_ + str.size_ >= capacity_) {
      double_size();
    }
    for (size_t i = size_; i < size_ + str.size_; ++i) {
      my_string[i] = str.my_string[i - size_];
    }
    size_ += str.length();
    return *this;
  }

  String operator+(char sym) const {
    String _result(*this);
    _result += sym;
    return _result;
  }

  String operator+(const String& str) const {
    String result(*this);
    result += str;
    return result;
  }

  char& operator[](size_t index) {
    return my_string[index];
  }

  char operator[](size_t index) const {
    return my_string[index];
  }

  size_t length() const {
    return size_;
  }

  char& front() {
    return my_string[0];
  }

  const char& front() const {
    return my_string[0];
  }

  char& back() {
    return my_string[size_ - 1];
  }

  const char& back() const {
    return my_string[size_ - 1];
  }

  String substr(size_t start, size_t count) const {
    String s(count, '0');
    for (size_t i = start; i < start + count; ++i) {
      s[i - start] = my_string[i];
    }
    return s;
  }

  size_t find(const String& s) const {
    size_t len = s.length();
    for (size_t i = 0; i <= size_ - len; ++i) {
      if (this->substr(i, len) == s) {
        return i;
      }
    }
    return size_;
  }

  size_t rfind(const String& s) const {
    size_t len = s.length();
    size_t ans = size_;
    for (size_t i = 0; i <= size_ - len; ++i) {
      if (this->substr(i, len) == s) {
        ans = i;
      }
    }
    return ans;
  }

  bool empty() {
    return size_ == 0;
  }

  void pop_back() {
    if (size_)
      size_--;
  }

  void push_back(char sym) {
    if (size_ >= capacity_) {
      double_size();
    }

    my_string[size_] = sym;
    size_++;
  }

  void clear() {
    delete[] my_string;
    size_ = 0;
    capacity_ = 10;
    my_string = nullptr;
  }

  void swap(String& _myString) {
    std::swap(my_string, _myString.my_string);
    std::swap(size_, _myString.size_);
    std::swap(capacity_, _myString.capacity_);
  }

  friend std::ostream& operator<<(std::ostream& out, const String& s);

  friend std::istream& operator>>(std::istream& in, String& s);

 private:
  char* my_string;
  size_t size_;
  size_t capacity_;

  void copy_method(const String& orig) {
    size_ = orig.size_;
    capacity_ = orig.capacity_;
    my_string = new char[capacity_];
    memcpy(my_string, orig.my_string, size_);
  }

  void double_size() {
    String _tmp(capacity_, '0');
    for (size_t i = 0; i < size_; ++i) {
      _tmp.my_string[i] = my_string[i];
    }
    _tmp.size_ = size_;
    _tmp.swap(*this);
  }

};

void swap(String& _left_operand, String& _right_operand) {
  _left_operand.swap(_right_operand);
}

String operator+(char _char, const String& _myString) {
  return String(1, _char) + _myString;
}

std::ostream& operator<<(std::ostream& out, const String& s) {
  for (size_t i = 0; i < s.size_; ++i) {
    out << s[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, String& s) {
  if (s.size_ != 0) {
    s.clear();
  }
  char c = 'c';
  while (in.get(c) && c != ' ' && !std::isspace(c)) {
    s.push_back(c);
  }
  return in;
}