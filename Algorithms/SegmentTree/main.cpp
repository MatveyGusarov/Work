#include <iostream>
#include <vector>

class Tree {
 public:
  void Get(std::vector<int64_t>& el, std::vector<int64_t>& fl, int64_t sh) {
    Get_(el, fl, sh);
  }
  void BuildMin() {
    BuildMin_();
  }
  void BuildMax() {
    BuildMax_();
  }
  void BuildBoard() {
    BuildBoard_();
  }
  void Review() {
    view();
  }
  int64_t GetShift() {
    return shift;
  }
  int64_t GetMin(int64_t left, int64_t rigth, int64_t parent) {
    if ((Tl[parent] == left)&&(Tr[parent] == rigth)) {
      return Min[parent];
    } else {
      int64_t left_child = (parent * 2 + 1);
      int64_t rigth_child = (parent * 2 + 2);
      if (Tl[rigth_child] <= left) {
        return GetMin(left, rigth, rigth_child);
      } else if (rigth <= Tr[left_child]) {
        return GetMin(left, rigth, left_child);
      } else {
        return std::min(GetMin(left, Tr[left_child], left_child), GetMin(Tl[rigth_child], rigth, rigth_child));
      }
    }
  }
  int64_t GetMax(int64_t left, int64_t rigth, int64_t parent) {
    if ((Tl[parent] == left)&&(Tr[parent] == rigth)) {
      return Max[parent];
    } else {
      int64_t left_child = (parent * 2 + 1);
      int64_t rigth_child = (parent * 2 + 2);
      if (Tl[rigth_child] <= left) {
        return GetMax(left, rigth, rigth_child);
      } else if (rigth <= Tr[left_child]) {
        return GetMax(left, rigth, left_child);
      } else {
        return std::max(GetMax(left, Tr[left_child], left_child), GetMax(Tl[rigth_child], rigth, rigth_child));
      }
    }
  }
  void ChangeDot(int64_t pos, int64_t val) {
    ChangeDot_(pos, val);
  }

 private:
  int64_t shift;
  std::vector<int64_t> elems;
  std::vector<int64_t> flags;
  std::vector<int64_t> Min;
  std::vector<int64_t> Max;
  std::vector<int64_t> Tl;
  std::vector<int64_t> Tr;

  void ChangeDot_(int64_t pos, int64_t val) {
    int64_t parent = shift + pos - 1;
    elems[parent] = val;
    Min[parent] = val;
    Max[parent] = val;
    while (parent > 0) {
      parent = (parent - 1) / 2;
      int64_t left_child = (parent * 2 + 1);
      int64_t rigth_child = (parent * 2 + 2);
      if (flags[rigth_child] != -1) {
        Min[parent] = std::min(Min[left_child], Min[rigth_child]);
      } else if (flags[left_child] != -1) {
        Min[parent] = Min[left_child];
      }
    }

    parent = shift + pos - 1;
    while (parent > 0) {
      parent = (parent - 1) / 2;
      int64_t left_child = (parent * 2 + 1);
      int64_t rigth_child = (parent * 2 + 2);
      if (flags[rigth_child] != -1) {
        Max[parent] = std::max(Max[left_child], Max[rigth_child]);
      } else if (flags[left_child] != -1) {
        Max[parent] = Max[left_child];
      }
    }
  }

  void Get_(std::vector<int64_t>& el, std::vector<int64_t>& fl, int64_t sh) {
    shift = sh;
    elems = el;
    flags = fl;
  }

  void BuildMin_() {
    Min = elems;
    int64_t beg = shift;
    int64_t end;
    while (beg > 0) {
      end = beg;
      beg = (beg - 1)/2;
      for (int i = beg; i < end; ++i) {
        int64_t left_child = 2 * i + 1;
        int64_t rigth_child = 2 * i + 2;
        if (flags[rigth_child] != -1) {
          Min[i] = std::min(Min[left_child], Min[rigth_child]);
        } else if (flags[left_child] != -1) {
          Min[i] = Min[left_child];
        } else {
          Min[i] = 50000;
        }
      }
    }
  }

  void BuildMax_() {
    Max = elems;
    int64_t beg = shift;
    int64_t end;
    while (beg > 0) {
      end = beg;
      beg = (beg - 1)/2;
      for (int i = beg; i < end; ++i) {
        int64_t left_child = 2 * i + 1;
        int64_t rigth_child = 2 * i + 2;
        if (flags[rigth_child] != -1) {
          Max[i] = std::max(Max[left_child], Max[rigth_child]);
        } else if (flags[left_child] != -1) {
          Max[i] = Max[left_child];
        } else {
          Max[i] = -1;
        }
      }
    }
  }

  void BuildBoard_() {
    Tl = elems;
    Tr = elems;
    int64_t beg;
    int64_t end;
    beg = shift;
    for (int64_t i = beg; i < elems.size(); ++i) {
      Tl[i] = i;
      Tr[i] = i;
    }
    while (beg > 0) {
      end = beg;
      beg = (beg - 1) / 2;
      for (int64_t i = beg; i < end; ++i) {
        int64_t left_child = (i * 2 + 1);
        int64_t rigth_child = (i * 2 + 2);
        Tl[i] = Tl[left_child];
        Tr[i] = Tr[rigth_child];
      }
    }
  }

  void view() {
    std::cout << "elems  : \n";
    for (int i = 0; i < 20; ++i) {
      std::cout << elems[i] << " ";
    }
    std::cout << "\n";
    std::cout << "flags  :  \n";
    for (int i = 0; i < 20; ++i) {
      std::cout << flags[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Min  :  \n";
    for (int i = 0; i < 20; ++i) {
      std::cout << Min[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Max  :  \n";
    for (int i = 0; i < 20; ++i) {
      std::cout << Max[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Tl  :  \n";
    for (int i = 0; i < 20; ++i) {
      std::cout << Tl[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Tr  :  \n";
    for (int i = 0; i < 20; ++i) {
      std::cout << Tr[i] << " ";
    }
    std::cout << "\n";
  }
};

void Commands(Tree& tree) {
  int query;
  std::cin >> query;
  for (int i = 0; i < query; ++i) {
    int64_t x;
    int64_t y;
    std::cin >> x;
    std::cin >> y;
    if (x > 0) {
      int64_t shift = tree.GetShift();
      int64_t minElem = tree.GetMin(x + shift - 1, y + shift - 1, 0);
      int64_t maxElem = tree.GetMax(x + shift - 1, y + shift - 1, 0);
      std::cout << (maxElem - minElem) << "\n";
    } else if (x < 0) {
      tree.ChangeDot(-x, y);
    } else {
      tree.Review();
    }
  }
}

void build(int64_t query) {
  int64_t que_2 = 1;
  while (que_2 < query) {
    que_2 = que_2 << 1;
  }
  std::vector<int64_t> elems;
  std::vector<int64_t> flags;

  for (int i = 0; i < (que_2 -1); ++i) {
    elems.push_back(0);
    flags.push_back(1);
  }
  for (int i = 1; i < que_2+1; ++i) {
    if (i < query + 1) {
      int64_t element;
      int64_t first_part;
      int64_t second_part;
      first_part = i % 12345;
      first_part = (first_part * (i % 12345)) % 12345;
      second_part = i % 23456;
      second_part = (second_part * (i % 23456)) % 23456;
      second_part = (second_part * (i % 23456)) % 23456;
      element = (first_part + second_part);
      elems.push_back(element);
      flags.push_back(1);
    } else {
      elems.push_back(0);
      flags.push_back(-1);
    }
  }

  Tree tree;
  tree.Get(elems, flags, que_2 -1);

  tree.BuildMin();
  tree.BuildMax();
  tree.BuildBoard();

  Commands(tree);
}

int main() {
  int64_t query = 100010;
  build(query);
}