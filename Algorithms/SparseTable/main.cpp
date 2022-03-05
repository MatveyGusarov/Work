#include <iostream>
#include <vector>

class SparseTable {
 public:
  SparseTable(int64_t n_size) {
    std::vector<int64_t> elems;
    std::vector<std::vector<int64_t>> vec;
    for (int64_t i = 0; i < n_size; ++i) {
      int64_t element;
      std::cin >> element;
      elems.push_back(element);
    }
    vec.emplace_back();
    for (int64_t i = 0; i < n_size; ++i) {
      vec[0].push_back(i);
    }

    int64_t log;
    int64_t len;
    log = 0;
    len = 1;
    while (len <= n_size) {
      len = len << 1;
      log += 1;
    }

    int64_t step = 1;
    for (int64_t j = 1; j < log; ++j) {
      vec.emplace_back();
      for (int64_t i = 0; i < n_size; ++i) {
        if (j == 1) {
          if ((i + 2 * step - 1) < n_size) {
            if (elems[i] < elems[i + 1]) {
              vec[j].push_back(i);
            } else {
              vec[j].push_back(i + step);
            }
          } else{
            vec[j].push_back(-1);
          }
        } else {
          if ((i + 2 * step - 1) < n_size) {
            if (elems[vec[j - 1][i]] < elems[vec[j - 1][i + step]]) {
              vec[j].push_back(vec[j - 1][i]);
            } else {
              vec[j].push_back(vec[j - 1][i + step]);
            }
          } else {
            vec[j].push_back(-1);
          }
        }
      }
      step = step * 2;
    }

    Build_(vec, elems, log);
  }
  int64_t Get(int64_t left, int64_t right) {
    int64_t length;
    int64_t log;
    length = 1;
    log = 0;
    while ((length * 2) < (right - left + 1)) {
      length = length << 1;
      log += 1;
    }

    int64_t first_index;
    int64_t second_index;
    first_index = sparse_[log][left];
    second_index = sparse_[log][right - length + 1];

    if (elems_[first_index] < elems_[second_index]) {
      return first_index;
    } else {
      return second_index;
    }
  }
  int64_t Tip(int64_t first, int64_t second) {
    std::cout << std::min(elems_[first], elems_[second]) << "\n";
  }
  void Review() {
    std::cout << "SPARSE_  :  \n";
    for (int64_t i = log_table_ - 1; i > -1; --i) {
      for (int64_t j = 0; j < elems_.size(); ++j) {
        std::cout << sparse_[i][j] << " ";
      }
      std::cout << "\n";
    }
    std::cout << "ELEMS_  :  \n";
    for (int64_t i = 0; i < elems_.size(); ++i) {
      std::cout << elems_[i] << " ";
    }
    std::cout << "\n";
  }
 private:
  std::vector<std::vector<int64_t>> sparse_;
  std::vector<int64_t> elems_;
  int64_t log_table_{};

  void Build_(std::vector<std::vector<int64_t>>& vec, std::vector<int64_t> elems, int64_t log) {
    sparse_ = vec;
    elems_ = elems;
    log_table_ = log;
  }
};

void Command(int64_t query, SparseTable& table) {
  for (int64_t i = 0; i < query; ++i) {
    int64_t left;
    int64_t right;
    std::cin >> left;
    std::cin >> right;

    if (left == -1) {
      table.Review();
    } else {
      int64_t index_rmq_1;
      index_rmq_1 = table.Get(left - 1, right - 1);
      index_rmq_1 += 1;
      if ((index_rmq_1 != left) && (index_rmq_1 != right)) {
        int64_t first;
        int64_t second;
        first = table.Get(left - 1, index_rmq_1 - 2);
        second = table.Get(index_rmq_1, right - 1);
        table.Tip(first, second);
      } else if (index_rmq_1 == left) {
        int64_t index_rmq_2;
        index_rmq_2 = table.Get(left, right - 1);
        table.Tip(index_rmq_1, index_rmq_2);
      } else if (index_rmq_1 == right) {
        int64_t index_rmq_2;
        index_rmq_2 = table.Get(left - 1, right - 2);
        table.Tip(index_rmq_2, index_rmq_2);
      }
    }
  }
}

int main() {
  int64_t n_size;
  int64_t query;
  std::cin >> n_size;
  std::cin >> query;
  SparseTable table(n_size);
  Command(query, table);
}
