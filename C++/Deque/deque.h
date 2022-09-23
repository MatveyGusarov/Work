#include <algorithm>
#include <iostream>
#include <iterator>

const int64_t kBase = 10;

template <typename T>
class Deque {
 private:
  T** blocks_ = nullptr;
  int64_t zero_pointer_;
  int64_t zero_index_;
  size_t size_;
  size_t levels_;

  void resize_shift() {
    size_t new_levels = levels_ * 2 + 1;
    int64_t new_zero_pointer =
        zero_pointer_ + (new_levels - 1) / 2 - (levels_ - 1) / 2;
    T** new_blocks = reinterpret_cast<T**>(
        new uint8_t[new_levels * sizeof(reinterpret_cast<T*>(
                                     new uint8_t[kBase * sizeof(T)]))]);

    for (size_t i = 0; i < new_levels; ++i) {
      if (i < ((new_levels - 1) / 2 - (levels_ - 1) / 2)) {
        new_blocks[i] = reinterpret_cast<T*>(new uint8_t[kBase * sizeof(T)]);
      } else if (i > ((new_levels - 1) / 2 + (levels_ - 1) / 2)) {
        new_blocks[i] = reinterpret_cast<T*>(new uint8_t[kBase * sizeof(T)]);
      } else {
        new_blocks[i] = blocks_[i - (new_levels - 1) / 2 + (levels_ - 1) / 2];
      }
    }

    delete[] blocks_;

    blocks_ = new_blocks;
    zero_pointer_ = new_zero_pointer;
    levels_ = new_levels;
  }

  std::pair<int64_t, int64_t> get_pair(int64_t index) const {  //  PERFECT!
    int64_t vector_number;
    int64_t in_vector_number;
    int64_t value;
    value = zero_pointer_ * kBase + zero_index_ + index;
    vector_number = value / kBase;
    in_vector_number = value - vector_number * kBase;
    return std::make_pair(vector_number, in_vector_number);
  }

 public:
  template <bool is_const>
  class BaseIterator {
   public:
    using difference_type = std::ptrdiff_t;                       //  NOLINT
    using value_type = std::conditional_t<is_const, const T, T>;  //  NOLINT
    using reference = value_type&;                                //  NOLINT
    using pointer = value_type*;                                  //  NOLINT
    using iterator_category = std::random_access_iterator_tag;    //  NOLINT

    explicit BaseIterator(T** ptr, int64_t point, int64_t index)
        : ptr_{ptr}, pointer_(point), index_(index) {}

    BaseIterator(const BaseIterator& other) {
      ptr_ = other.ptr_;
      pointer_ = other.pointer_;
      index_ = other.index_;
    }

    BaseIterator& operator=(const BaseIterator& other) {
      ptr_ = other.ptr_;
      pointer_ = other.pointer_;
      index_ = other.index_;
      return *this;
    }

    BaseIterator& operator++() {
      *this = get_next(*this, 1);
      return *this;
    }

    BaseIterator operator++(int) {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    BaseIterator& operator--() {
      *this = get_prev(*this, 1);
      return *this;
    }

    BaseIterator& operator+=(int64_t diff) {
      *this = get_next(*this, diff);
      return *this;
    }

    BaseIterator& operator-=(int64_t diff) {
      *this = get_prev(*this, diff);
      return *this;
    }

    int64_t distance_my(const BaseIterator<is_const> kOther) const {
      int64_t dist = (pointer_ * kBase + index_) -
                     (kOther.pointer_ * kBase + kOther.index_);
      if (dist >= 0) {
        return dist;
      }
      return (-dist);
    }

    bool operator==(BaseIterator<is_const> right) {
      return (pointer_ * kBase + index_) ==
             (right.pointer_ * kBase + right.index_);
    }

    bool operator!=(BaseIterator<is_const> right) {
      return (pointer_ * kBase + index_) !=
             (right.pointer_ * kBase + right.index_);
    }

    bool operator<=(BaseIterator<is_const> right) {
      return (pointer_ * kBase + index_) <=
             (right.pointer_ * kBase + right.index_);
    }

    bool operator>=(BaseIterator<is_const> right) {
      return (pointer_ * kBase + index_) >=
             (right.pointer_ * kBase + right.index_);
    }

    bool operator>(BaseIterator<is_const> right) {
      return (*this >= right) && (*this != right);
    }

    bool operator<(BaseIterator<is_const> right) {
      return (*this <= right) && (*this != right);
    }

    std::pair<int64_t, int64_t> get_body() const {
      return std::make_pair(pointer_, index_);
    }

    operator Deque<T>::BaseIterator<true>() {
      return Deque<T>::BaseIterator<true>(ptr_ + index_);
    };

    pointer operator->() const { return ptr_[pointer_] + index_; }
    reference operator*() { return *(ptr_[pointer_] + index_); }

   private:
    T** ptr_;
    int64_t pointer_;
    int64_t index_;

    BaseIterator<is_const> get_next(BaseIterator<is_const> iter, int64_t diff) {
      Deque<T>::BaseIterator<is_const> it(
          iter.ptr_, iter.pointer_ + (iter.index_ + diff) / kBase,
          (iter.index_ + diff) % kBase);
      return it;
    }

    BaseIterator<is_const> get_prev(BaseIterator<is_const> iter, int64_t diff) {
      int64_t p = static_cast<int64_t>(iter.index_) - diff;
      if (p >= 0) {
        Deque<T>::BaseIterator<is_const> it(
            iter.ptr_, iter.pointer_ + p / kBase, p % kBase);
        return it;
      }
      Deque<T>::BaseIterator<is_const> it(
          iter.ptr_, (iter.pointer_ * kBase - diff) / kBase,
          (iter.pointer_ * kBase + iter.index_ - diff) % kBase);
      return it;
    }
  };

  using iterator = BaseIterator<false>;  //  NOLINT
  using ConstIterator = BaseIterator<true>;
  using ReverseIterator = std::reverse_iterator<iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

  ConstIterator begin() const {
    BaseIterator<true> iter(blocks_, zero_pointer_, zero_index_);
    return iter;
  }

  iterator begin() {
    BaseIterator<false> iter(blocks_, zero_pointer_, zero_index_);
    return iter;
  }

  ReverseIterator rend() {
    BaseIterator<false> iter(begin());
    return std::reverse_iterator<BaseIterator<false>>(iter);
  }

  ConstIterator cbegin() const {
    BaseIterator<true> iter(blocks_, zero_pointer_, zero_index_);
    return iter;
  }

  ReverseIterator rbegin() {
    BaseIterator<false> iter(end());
    return std::reverse_iterator<BaseIterator<false>>(iter);
  }

  iterator end() {
    BaseIterator<false> iter(blocks_,
                             zero_pointer_ + (zero_index_ + size_) / kBase,
                             (zero_index_ + size_) % kBase);
    return iter;
  }

  ConstIterator end() const {
    BaseIterator<true> iter(blocks_,
                            zero_pointer_ + (zero_index_ + size_) / kBase,
                            (zero_index_ + size_) % kBase);
    return iter;
  }

  ConstIterator cend() const {
    BaseIterator<true> iter(blocks_,
                            zero_pointer_ + (zero_index_ + size_) / kBase,
                            (zero_index_ + size_) % kBase);
    return iter;
  }

  template <bool is_const>
  void erase(const BaseIterator<is_const> kIter) {
    std::pair<int64_t, int64_t> p = kIter.get_body();
    shift_erase(p);
    size_ -= 1;
  }

  template <bool is_const>
  void insert(const BaseIterator<is_const> kIter, const T& object) {
    if ((zero_pointer_ * kBase + zero_index_ + size_) ==
        (levels_ * kBase - 1)) {
      std::pair<int64_t, int64_t> p = kIter.get_body();
      shift_insert(p);
      blocks_[p.first][p.second] = object;
      size_ += 1;
      resize_shift();
      return;
    }
    std::pair<int64_t, int64_t> p = kIter.get_body();
    shift_insert(p);
    blocks_[p.first][p.second] = object;
    size_ += 1;
  }

  Deque() {
    blocks_ = reinterpret_cast<T**>(
        new uint8_t*[1 * sizeof(reinterpret_cast<T*>(
                             new uint8_t[kBase * sizeof(T)]))]);
    blocks_[0] = reinterpret_cast<T*>(new uint8_t[kBase * sizeof(T)]);
    zero_pointer_ = 0;
    zero_index_ = 0;
    size_ = 0;
    levels_ = 1;
  }

  Deque(const Deque& deq) {
    size_ = deq.size_;
    zero_pointer_ = deq.zero_pointer_;
    zero_index_ = deq.zero_index_;
    levels_ = deq.levels_;

    blocks_ = reinterpret_cast<T**>(
        new uint8_t*[levels_ * sizeof(reinterpret_cast<T*>(
                                   new uint8_t[kBase * sizeof(T)]))]);
    for (size_t i = 0; i < levels_; ++i) {
      T* buffer = reinterpret_cast<T*>(new uint8_t*[kBase * sizeof(T)]);
      for (size_t j = 0; j < static_cast<size_t>(kBase); ++j) {
        new (buffer + j) T(deq[i * static_cast<size_t>(kBase) + j]);
      }
      new (blocks_ + i) T*(buffer);
    }
  }

  Deque& operator=(const Deque<T>& deq) {
    for (iterator it = begin(); it < end(); ++it) {
      it->~T();
    }
    for (size_t i = 0; i < levels_; ++i) {
      delete[] reinterpret_cast<uint8_t*>(blocks_[i]);
    }
    delete[] blocks_;

    size_ = deq.size_;
    zero_pointer_ = deq.zero_pointer_;
    zero_index_ = deq.zero_index_;
    levels_ = deq.levels_;

    blocks_ = reinterpret_cast<T**>(
        new uint8_t*[levels_ * sizeof(reinterpret_cast<T*>(
                                   new uint8_t[kBase * sizeof(T)]))]);
    for (size_t i = 0; i < levels_; ++i) {
      T* buffer = reinterpret_cast<T*>(new uint8_t*[kBase * sizeof(T)]);
      for (size_t j = 0; j < static_cast<size_t>(kBase); ++j) {
        new (buffer + j) T(deq[i * kBase + j]);
      }
      new (blocks_ + i) T*(buffer);
    }

    return *this;
  }

  ~Deque() {
    for (iterator it = begin(); it < end(); ++it) {
      it->~T();
    }
    for (size_t i = 0; i < levels_; ++i) {
      delete[] reinterpret_cast<uint8_t*>(blocks_[i]);
    }
    delete[] blocks_;
  }

  Deque(int x) {
    size_ = x;
    levels_ = x / kBase + 2;
    levels_ += (levels_ + 1) % 2;

    blocks_ = reinterpret_cast<T**>(
        new uint8_t*[levels_ * sizeof(reinterpret_cast<T*>(
                                   new uint8_t[kBase * sizeof(T)]))]);
    zero_pointer_ = 0;
    zero_index_ = 0;
    for (size_t i = 0; i < levels_; ++i) {
      blocks_[i] = reinterpret_cast<T*>(new uint8_t[kBase * sizeof(T)]);
    }

    for (auto it = begin(); it != end(); ++it) {
      std::pair<int64_t, int64_t> par;
      par = it.get_body();
      blocks_[par.first][par.second] = T();
    }
  }

  Deque(int x, const T& object) {
    size_ = x;
    levels_ = x / kBase + 2;
    levels_ += (levels_ + 1) % 2;
    blocks_ = reinterpret_cast<T**>(
        new uint8_t*[levels_ * sizeof(reinterpret_cast<T*>(
                                   new uint8_t[kBase * sizeof(T)]))]);
    zero_pointer_ = 0;
    zero_index_ = 0;
    for (size_t i = static_cast<size_t>(zero_pointer_); i < levels_; ++i) {
      blocks_[i] = reinterpret_cast<T*>(new uint8_t[kBase * sizeof(T)]);
      for (size_t j = 0; j < static_cast<size_t>(kBase); ++j) {
        if ((i * static_cast<size_t>(kBase) + j) < static_cast<size_t>(x)) {
          blocks_[i][j] = object;
        }
      }
    }
  }

  size_t size() const { return size_; }

  T& at(size_t index) {
    if (index >= size_) {
      throw std::out_of_range("error");
    }
    std::pair<int64_t, int64_t> p = get_pair(index);
    return blocks_[p.first][p.second];
  }

  const T& at(size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("error");
    }
    std::pair<int64_t, int64_t> p = get_pair(index);
    return blocks_[p.first][p.second];
  }

  T& operator[](int64_t index) {
    std::pair<int64_t, int64_t> p = get_pair(index);
    return blocks_[p.first][p.second];
  }

  const T& operator[](int64_t index) const {
    std::pair<int64_t, int64_t> p = get_pair(index);
    return blocks_[p.first][p.second];
  }

  void push_front(T elem) {
    if ((zero_pointer_ == 0) && (zero_index_ == 0)) {
      resize_shift();
    }
    if (zero_index_ == 0) {
      zero_pointer_ -= 1;
      zero_index_ = kBase - 1;
    } else {
      zero_index_ -= 1;
    }
    blocks_[zero_pointer_][zero_index_] = elem;
    size_ += 1;
  }

  void push_back(T elem) {
    if (zero_pointer_ + (zero_index_ + size_) / kBase >= levels_) {
      resize_shift();
    }

    int64_t end_position = zero_pointer_ + (zero_index_ + size_) / kBase;
    int64_t end_index = (zero_index_ + size_) % kBase;
    blocks_[end_position][end_index] = elem;
    size_ += 1;
  }

  void pop_front() {
    if (size_ > 0) {
      iterator it = begin();
      it->~T();
      if ((zero_index_) + 1 < kBase) {
        zero_index_ += 1;
      } else {
        zero_index_ = 0;
        zero_pointer_ += 1;
      }
      size_ -= 1;
    }
  }

  void pop_back() {
    if (size_ > 0) {
      iterator it = (--(end()));
      it->~T();
      size_ -= 1;
    }
  }

  void shift_erase(std::pair<int64_t, int64_t> p) {
    std::pair<int64_t, int64_t> p_copy = p;
    std::pair<int64_t, int64_t> p_end;
    p_end = std::make_pair(
        (zero_pointer_ * kBase + zero_index_ + size_ - 1) / kBase,
        (zero_index_ + size_ - 1) % kBase);
    while (p_copy != p_end) {
      blocks_[p_copy.first][p_copy.second] =
          blocks_[(p_copy.first * kBase + p_copy.second + 1) / kBase]
                 [(p_copy.second + 1) % kBase];
      p_copy =
          std::make_pair((p_copy.first * kBase + p_copy.second + 1) / kBase,
                         (p_copy.second + 1) % kBase);
    }
  }

  void shift_insert(std::pair<int64_t, int64_t> p) {
    std::pair<int64_t, int64_t> p_end = p;
    std::pair<int64_t, int64_t> p_copy;
    p_copy =
        std::make_pair((zero_pointer_ * kBase + zero_index_ + size_) / kBase,
                       (zero_index_ + size_) % kBase);
    while (p_copy != p_end) {
      if (p_copy.second > 0) {
        blocks_[p_copy.first][p_copy.second] =
            blocks_[p_copy.first][p_copy.second - 1];
        p_copy = std::make_pair(p_copy.first, p_copy.second - 1);
      } else {
        blocks_[p_copy.first][p_copy.second] =
            blocks_[p_copy.first - 1][kBase - 1];
        p_copy = std::make_pair(p_copy.first - 1, kBase - 1);
      }
    }
  }

  template <bool is_const>
  friend BaseIterator<is_const> operator+(const BaseIterator<is_const>& iter,
                                          int64_t diff) {
    Deque<T>::BaseIterator<is_const> copy(iter);
    copy += diff;
    return copy;
  }

  template <bool is_const>
  friend BaseIterator<is_const> operator+(int64_t diff,
                                          const BaseIterator<is_const>& iter) {
    Deque<T>::iterator copy(iter);
    copy += diff;
    return copy;
  }

  template <bool is_const>
  friend BaseIterator<is_const> operator-(const BaseIterator<is_const>& iter,
                                          int64_t diff) {
    Deque<T>::BaseIterator<is_const> copy(iter);
    copy -= diff;
    return copy;
  }

  template <bool is_const = false>
  friend int64_t operator-(const BaseIterator<is_const>& left,
                           const BaseIterator<is_const>& right) {
    return left.distance_my(right);
  }

  template <bool is_const = false>
  friend BaseIterator<is_const> operator<(const BaseIterator<is_const>& left,
                                          const BaseIterator<is_const>& right) {
    Deque<T>::BaseIterator<false> it(left);
    return ++it <= right;
  }

  template <bool is_const>
  friend BaseIterator<is_const> operator>(const BaseIterator<is_const>& left,
                                          const BaseIterator<is_const>& right) {
    Deque<T>::BaseIterator<false> it(left);
    return --it >= right;
  }
};
