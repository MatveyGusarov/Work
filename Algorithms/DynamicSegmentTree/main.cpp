#include <iostream>
#include <vector>

const int64_t kMaxK = 1073741823;

class Tree {
 public:
  Tree() { head_ = nullptr; }

  void Put(int64_t element) { PutPriv(element, head_, 0); }

  int64_t GetSum(int64_t element) {
    int64_t sum;
    sum = GetSumPriv(0, element, head_, 0);
    return sum;
  }

 private:
  struct Node {
    Node() {
      left = -1;
      right = -1;
      left_child = nullptr;
      right_child = nullptr;
      sum = 0;
    }

    int64_t left;
    int64_t right;
    int64_t sum;
    Node* left_child;
    Node* right_child;
  };
  Node* head_ = nullptr;

  void PutPriv(int64_t element, Node* par, int64_t parent) {
    if (head_ == nullptr) {
      head_ = new Node();
      head_->left = 0;
      head_->right = kMaxK;
    }

    if (parent == 0) {
      par = head_;
    }

    par->sum += element;
    if (par->left != par->right) {
      int64_t center;
      center = (par->left + par->right - 1) / 2;
      if (element < center + 1) {
        CreateLeftChild(par, center);
        PutPriv(element, par->left_child, parent * 2 + 1);
      } else if (element > center) {
        CreateRightChild(par, center);
        PutPriv(element, par->right_child, parent * 2 + 2);
      }
    }
  }

  int64_t GetSumPriv(int64_t left, int64_t right, Node* par, int64_t parent) {
    if (head_ == nullptr) {
      head_ = new Node();
      head_->left = 0;
      head_->right = kMaxK;
    }

    if (parent == 0) {
      par = head_;
    }

    if ((left == par->left) && (right == par->right)) {
      return par->sum;
    }
    int64_t center;
    center = (par->left + par->right - 1) / 2;
    CreateLeftChild(par, center);
    CreateRightChild(par, center);
    if (right < center + 1) {
      return GetSumPriv(left, right, par->left_child, parent * 2 + 1);
    }
    if (left > center) {
      return GetSumPriv(left, right, par->right_child, parent * 2 + 2);
    }
    return GetSumPriv(left, center, par->left_child, parent * 2 + 1) +
        GetSumPriv(center + 1, right, par->right_child, parent * 2 + 2);
  }

  static void CreateLeftChild(Node* par, int64_t center) {
    if (par->left_child == nullptr) {
      par->left_child = new Node();
      par->left_child->left = par->left;
      par->left_child->right = center;
    }
  }

  static void CreateRightChild(Node* par, int64_t center) {
    if (par->right_child == nullptr) {
      par->right_child = new Node();
      par->right_child->left = center + 1;
      par->right_child->right = par->right;
    }
  }
};

void Commands(int64_t query) {
  Tree tree;
  for (int64_t i = 0; i < query; ++i) {
    std::string s;
    std::cin >> s;
    int64_t element;
    std::cin >> element;
    if (s == "+") {
      tree.Put(element);
    } else {
      int64_t sum;
      sum = tree.GetSum(element);
      std::cout << sum << "\n";
    }
  }
}

int main() {
  int64_t query;
  std::cin >> query;
  Commands(query);
}