#include <iostream>

const int MaxResult = 1000000100;
const int MinResult = 1000000000;

class SplayTree {
 public:
  void Insert(int64_t key) {
    if (head_ == nullptr) {
      head_ = NewNode(key);
    } else {
      bool flag;
      flag = Find(head_, key);
      if (!flag) {
        Add(head_, key);
        head_ = Splay(head_, key);
      }
    }
  }

  int64_t GetSum(int64_t key_left, int64_t key_right) {
    int64_t ans;
    int64_t res = MaxResult;
    Next(head_, key_left, res);
    key_left = res;
    if (key_left != MaxResult) {
      std::pair<Node*, Node*> pair_one;
      pair_one = Split(head_, key_left, false);
      res = MaxResult;
      Next(pair_one.second, key_right + 1, res);
      bool flag = false;
      if (res != MaxResult) {
        key_right = res;
      } else {
        key_right = GetMax(pair_one.second)->key;
        flag = true;
      }
      std::pair<Node*, Node*> pair_two;
      pair_two = Split(pair_one.second, key_right, flag);
      if (pair_two.first != nullptr) {
        ans = pair_two.first->sum;
      } else {
        ans = 0;
      }
      head_ = Merge(pair_one.first, Merge(pair_two.first, pair_two.second));
    } else {
      ans = 0;
    }
    return ans;
  }

  ~SplayTree() { Destructor(head_); }

 private:
  struct Node {
    ~Node() {
      left = nullptr;
      right = nullptr;
    }
    int64_t key;
    int64_t sum;
    Node* left;
    Node* right;
  };
  Node* head_ = nullptr;

  void Destructor(Node* parent) {
    if (parent != nullptr) {
      if (parent->left != nullptr) {
        Destructor(parent->left);
      }
      if (parent->right != nullptr) {
        Destructor(parent->right);
      }
      delete parent;
    }
  }

  void Next(Node* nod, int64_t val, int64_t& result) {
    if (nod != nullptr) {
      if (nod->key < val) {
        if (nod->right != nullptr) {
          Next(nod->right, val, result);
        }
      } else {
        if (nod->key < result) {
          result = nod->key;
        }

        if (nod->left != nullptr) {
          Next(nod->left, val, result);
        }
      }
    }
  }

  bool Find(Node* parent, int64_t x) {
    if (parent == nullptr) {
      return false;
    }
    if (parent->key == x) {
      return true;
    }
    if (parent->key > x) {
      return Find(parent->left, x);
    }
    return Find(parent->right, x);
  }

  std::pair<Node*, Node*> Split(Node* parent, int64_t key, bool flag) {
    if (parent == nullptr) {
      return std::make_pair(nullptr, nullptr);
    }
    parent = Splay(parent, key);
    if (!flag) {
      Node* l = parent->left;
      parent->left = nullptr;
      FixSizes(parent);
      Node* r = parent;
      return std::make_pair(l, r);
    }
    Node* r = parent->right;
    parent->right = nullptr;
    FixSizes(parent);
    Node* l = parent;
    return std::make_pair(l, r);
  }

  Node* Merge(Node* l, Node* r) {
    if (l == nullptr) {
      l = r;
    } else {
      if (r != nullptr) {
        Node* max_in_l = GetMax(l);
        Node* min_in_r = GetMin(r);
        l = Splay(l, max_in_l->key);
        r = Splay(r, min_in_r->key);
        l->right = r;
      }
    }

    FixSizes(l);
    return l;
  }

  Node* GetMin(Node* parent) {
    if ((parent == nullptr) || (parent->left == nullptr)) {
      return parent;
    }
    return GetMin(parent->left);
  }

  Node* GetMax(Node* parent) {
    if ((parent == nullptr) || (parent->right == nullptr)) {
      return parent;
    }
    return GetMax(parent->right);
  }

  static void FixSizes(Node* parent) {
    if (parent != nullptr) {
      int64_t sum_left;
      if (parent->left != nullptr) {
        sum_left = parent->left->sum;
      } else {
        sum_left = 0;
      }

      int64_t sum_right;
      if (parent->right != nullptr) {
        sum_right = parent->right->sum;
      } else {
        sum_right = 0;
      }

      parent->sum = sum_left + sum_right + parent->key;
    }
  }

  void Add(Node* parent, int64_t key) {
    if (parent->key > key) {
      if (parent->left != nullptr) {
        Add(parent->left, key);
        FixSizes(parent);
      } else {
        parent->left = NewNode(key);
      }
    } else {
      if (parent->right != nullptr) {
        Add(parent->right, key);
        FixSizes(parent);
      } else {
        parent->right = NewNode(key);
      }
    }
  }

  static Node* NewNode(int64_t key) {
    Node* vertex = new Node();
    vertex->key = key;
    vertex->sum = key;
    vertex->left = nullptr;
    vertex->right = nullptr;
    return (vertex);
  }

  static Node* RightRotate(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    FixSizes(y->right);
    FixSizes(y);
    return y;
  }

  static Node* LeftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    FixSizes(y->left);
    FixSizes(y);
    return y;
  }

  Node* Splay(Node* root, int64_t key) {
    if (root == nullptr || root->key == key) {
      return root;
    }

    if (root->key > key) {
      return SplayLeft(root, key);
    }

    if (root->right == nullptr) {
      return root;
    }
    if (root->right->key > key) {
      root->right->left = Splay(root->right->left, key);
      if (root->right->left != nullptr) {
        root->right = RightRotate(root->right);
      }
    } else if (root->right->key < key) {
      root->right->right = Splay(root->right->right, key);
      root = LeftRotate(root);
    }
    return (root->right == nullptr) ? root : LeftRotate(root);
  }

  Node* SplayLeft(Node* root, int64_t key) {
    if (root->left == nullptr) {
      return root;
    }
    if (root->left->key > key) {
      root->left->left = Splay(root->left->left, key);
      root = RightRotate(root);
    } else if (root->left->key < key) {
      root->left->right = Splay(root->left->right, key);
      if (root->left->right != nullptr) {
        root->left = LeftRotate(root->left);
      }
    }
    return (root->left == nullptr) ? root : RightRotate(root);
  }
};

void Commands(SplayTree& tree) {
  int64_t query;
  std::cin >> query;

  int64_t ans;
  bool flag = false;
  for (int64_t i = 0; i < query; ++i) {
    std::string s;
    std::cin >> s;
    if (s == "+") {
      int64_t elem;
      std::cin >> elem;
      if (flag) {
        elem = (elem + ans) % MinResult;
        flag = false;
      }
      tree.Insert(elem);
    } else {
      int64_t left;
      int64_t right;
      std::cin >> left;
      std::cin >> right;
      ans = tree.GetSum(left, right);
      std::cout << ans << "\n";
      flag = true;
    }
  }
}

int main() {
  SplayTree tree;
  Commands(tree);
}
