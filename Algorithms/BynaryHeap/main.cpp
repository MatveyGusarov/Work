#include <iostream>
#include <vector>

class BinaryHeap {
 public:
  void insert(int64_t x) {
    insert_(x);
  }

  void getMin() {
    getMin_();
  }

  void extractMin() {
    extractMin_();
  }

  void decreaseKey(int64_t i, int64_t delta) {
    decreaseKey_(i, delta);
  }
 private:
  std::vector<int64_t> Elem;
  std::vector<int64_t> Ind;
  std::vector<int64_t> Task;

  void Swapper(int64_t first_index_in_Elem, int64_t second_index_in_Elem) {
    int first_index_in_Task;
    int second_index_in_Task;
    first_index_in_Task = Ind[first_index_in_Elem];
    second_index_in_Task = Ind[second_index_in_Elem];
    std::swap(Elem[first_index_in_Elem], Elem[second_index_in_Elem]);
    std::swap(Task[first_index_in_Task], Task[second_index_in_Task]);
    std::swap(Ind[first_index_in_Elem], Ind[second_index_in_Elem]);
  }

  void siftUp(int64_t index) {
    int64_t newindex;
    while (index > 0) {
      if (index % 2 == 0) {
        newindex = (index - 2) / 2;
      } else {
        newindex = (index - 1) / 2;
      }
      if (Elem[index] < Elem[newindex]) {
        Swapper(index, newindex);
        index = newindex;
      } else {
        break;
      }
    }
  }

  void siftDown(int64_t index) {
    while ((2 * index + 1) < Elem.size()) {
      if ((2 * index + 2) < Elem.size()) {
        int left_child = 2 * index + 1;
        int rigth_child = 2 * index + 2;
        if ((Elem[left_child] < Elem[rigth_child])&&(Elem[index] > Elem[left_child])) {
          Swapper(index, left_child);
          index = left_child;
        } else if ((Elem[rigth_child] < Elem[left_child])&&(Elem[index] > Elem[rigth_child])) {
          Swapper(index, rigth_child);
          index = rigth_child;
        } else {
          break;
        }
      } else {
        int left_child = 2 * index + 1;
        if (Elem[index] > Elem[left_child]) {
          Swapper(index, left_child);
          index = left_child;
        } else {
          break;
        }
      }
    }
  }

  void insert_(int element) {
    Elem.push_back(element);
    Task.push_back(Elem.size()-1);
    Ind.push_back(Task.size()-1);
    siftUp(Elem.size() - 1);
  }

  void extractMin_() {
    Swapper(0, Elem.size()-1);
    Elem.pop_back();
    Ind.pop_back();
    siftDown(0);
    Task.push_back(-1);
  }

  void getMin_() {
    std::cout << Elem[0] << "\n";
    Task.push_back(-1);
  }

  void decreaseKey_(int64_t i, int64_t delta) {
    int64_t index_in_Elem = Task[i-1];
    Elem[index_in_Elem] -= delta;
    siftUp(index_in_Elem);
    Task.push_back(-1);
  }
};

void Commands(BinaryHeap& Heap) {
  int n;
  std::string command;
  std::cin >> n;
  for (int iterator = 0; iterator < n; ++iterator) {
    std::cin >> command;
    if (command == "insert") {
      int64_t x;
      std::cin >> x;
      Heap.insert(x);
    } else if (command == "getMin") {
      Heap.getMin();
    } else if (command == "extractMin") {
      Heap.extractMin();
    } else if (command == "decreaseKey") {
      int64_t i;
      int64_t delta;
      std::cin >> i;
      std::cin >> delta;
      Heap.decreaseKey(i, delta);
    }
  }
}

int main() {
  BinaryHeap Heap;
  Commands(Heap);
}
