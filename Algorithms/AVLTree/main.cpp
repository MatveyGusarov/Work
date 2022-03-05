#include <iostream>

const int kLl = 1000000100;
const int kPp = -1000000100;

class AVL {
 public:
  void Insert(int val) {
    if (!Exists(val)) {
      head_ = InsertPriv(head_, val);
    }
  }

  void Remove(int val) { head_ = Delete(head_, val); }

  bool Exists(int val) { return ExistsPriv(head_, val); }

  int Prev(int val) {
    int result = kPp;
    if (head_ == nullptr) {
      return result;
    }
    PrevPrivate(head_, val, result);
    return result;
  }

  int Next(int val) {
    int result = kLl;
    if (head_ == nullptr) {
      return result;
    }
    NextPrivate(head_, val, result);
    return result;
  }

  ~AVL() { Destructor(head_); }

 private:
  struct Node {
    explicit Node(int k) {
      value = k;
      height = 1;
    }
    ~Node() {
      left = nullptr;
      right = nullptr;
    }
    int value;
    int height;
    Node* left = nullptr;
    Node* right = nullptr;
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

  bool ExistsPriv(Node* parent, int val) {
    if (parent == nullptr) {
      return false;
    }
    if (parent->value == val) {
      return true;
    }
    if ((parent->value < val) && (parent->right != nullptr)) {
      return ExistsPriv(parent->right, val);
    }
    if ((parent->value > val) && (parent->left != nullptr)) {
      return ExistsPriv(parent->left, val);
    }
    return false;
  }

  void PrevPrivate(Node* parent, int val, int& result) {
    if (parent->value >= val) {
      if (parent->left != nullptr) {
        PrevPrivate(parent->left, val, result);
      }
    } else {
      if (parent->value > result) {
        result = parent->value;
      }

      if (parent->right != nullptr) {
        PrevPrivate(parent->right, val, result);
      }
    }
  }

  void NextPrivate(Node* parent, int val, int& result) {
    if (parent->value <= val) {
      if (parent->right != nullptr) {
        NextPrivate(parent->right, val, result);
      }
    } else {
      if (parent->value < result) {
        result = parent->value;
      }

      if (parent->left != nullptr) {
        NextPrivate(parent->left, val, result);
      }
    }
  }

  Node* InsertPriv(Node* parent, int val) {
    if (parent == nullptr) {
      return new Node(val);
    }
    if (parent->value < val) {
      parent->right = InsertPriv(parent->right, val);
    }
    if (parent->value > val) {
      parent->left = InsertPriv(parent->left, val);
    }

    return Balance(parent);
  }

  Node* Delete(Node* parent, int val) {
    if (parent == nullptr) {
      return nullptr;
    }
    if (val < parent->value) {
      parent->left = Delete(parent->left, val);
    } else if (val > parent->value) {
      parent->right = Delete(parent->right, val);
    } else {
      Node* node_left_son = parent->left;
      Node* node_right_son = parent->right;
      delete parent;
      if (node_right_son == nullptr) {
        return node_left_son;
      }

      Node* minimum_in_node_right_son = GetMinimum(node_right_son);
      minimum_in_node_right_son->right = DeleteMinimum(node_right_son);
      minimum_in_node_right_son->left = node_left_son;
      return Balance(minimum_in_node_right_son);
    }
    return Balance(parent);
  }

  Node* GetMinimum(Node* parent) {
    return parent->left != nullptr ? GetMinimum(parent->left) : parent;
  }

  Node* DeleteMinimum(Node* parent) {
    if (parent->left == nullptr) {
      return parent->right;
    }
    parent->left = DeleteMinimum(parent->left);
    return Balance(parent);
  }

  static Node* Balance(Node* parent) {
    SetHeight(parent);
    if (BalanceFactor(parent) == 2) {
      if (BalanceFactor(parent->right) < 0) {
        parent->right = RightRotation(parent->right);
      }
      return LeftRotation(parent);
    }
    if (BalanceFactor(parent) == -2) {
      if (BalanceFactor(parent->left) > 0) {
        parent->left = LeftRotation(parent->left);
      }
      return RightRotation(parent);
    }
    return parent;
  }

  static int GetHeight(Node* parent) {
    return parent != nullptr ? parent->height : 0;
  }

  static int BalanceFactor(Node* parent) {
    return GetHeight(parent->right) - GetHeight(parent->left);
  }

  static void SetHeight(Node* parent) {
    int height_left = GetHeight(parent->left);
    int height_right = GetHeight(parent->right);
    parent->height =
        1 + (height_left > height_right ? height_left : height_right);
  }

  static Node* RightRotation(Node* parent) {
    Node* parent_left_child = parent->left;
    parent->left = parent_left_child->right;
    parent_left_child->right = parent;
    SetHeight(parent);
    SetHeight(parent_left_child);
    return parent_left_child;
  }

  static Node* LeftRotation(Node* parent) {
    Node* parent_right_child = parent->right;
    parent->right = parent_right_child->left;
    parent_right_child->left = parent;
    SetHeight(parent);
    SetHeight(parent_right_child);
    return parent_right_child;
  }
};

void Commands(AVL& tree) {
  std::string s;
  while (std::cin >> s) {
    int key;
    std::cin >> key;
    if (s == "insert") {
      tree.Insert(key);
    } else if (s == "delete") {
      tree.Remove(key);
    } else if (s == "exists") {
      if (tree.Exists(key)) {
        std::cout << "true\n";
      } else {
        std::cout << "false\n";
      }
    } else if (s == "next") {
      if ((tree.Next(key) != kLl) && (tree.Next(key) != kPp)) {
        std::cout << tree.Next(key) << "\n";
      } else {
        std::cout << "none\n";
      }
    } else if (s == "prev") {
      if ((tree.Prev(key) != kLl) && (tree.Prev(key) != kPp)) {
        std::cout << tree.Prev(key) << "\n";
      } else {
        std::cout << "none\n";
      }
    }
  }
}

int main() {
  AVL tree;
  Commands(tree);
}