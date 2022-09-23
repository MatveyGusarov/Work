#include <iostream>
#include <cmath>
#include <functional>
#include <cstddef>
#include <vector>
#include <iterator>
#include <optional>
#include <type_traits>


template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct NodeBase;
  struct Node;

  template <typename IteratorType>
  class baseIterator;

  struct NodeBase {
    NodeBase(NodeBase* left, NodeBase* right) : prev(left), next(right) {}
    NodeBase* prev;
    NodeBase* next;
  };

  struct Node : public NodeBase {
    Node* next = nullptr;
    Node* prev = nullptr;
    T data;

    Node() = default;
    Node(const T& value): NodeBase(nullptr, nullptr), data(value) {}
  };

  Node* beginNode = nullptr;
  Node* endNode = nullptr;
  size_t listSize = 0;
  Allocator allocator_;

  void clear();
  void copy(const List& another);

  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc;
  using AllocTraitsT = std::allocator_traits<Allocator>;

 public:
  using value_type = T;
  using iterator = baseIterator<T>;
  using const_iterator = baseIterator<const T>;
  using reverse_iterator = std::reverse_iterator<baseIterator<T>>;
  using const_reverse_iterator = std::reverse_iterator<baseIterator<const T>>;

  explicit List(const Allocator& alloc = Allocator());
  List(size_t count, const T& value, const Allocator& alloc = Allocator());
  List(size_t count, const Allocator& alloc = Allocator());

  List(const List& another);
  List(List&& another);

  List& operator=(const List& right);
  List& operator=(List&& right);
  ~List();

  size_t size() const noexcept {
    return listSize;
  }
  void pop_front() noexcept;
  void pop_back() noexcept;


  template <typename U>
  void push_front(U&& value);

  template <typename U>
  void push_back(U&& value);

  Allocator get_allocator() const noexcept;

  const_iterator begin() const {return const_iterator(beginNode);}
  const_iterator cbegin() const {return const_iterator(beginNode);}
  const_iterator end() const {return const_iterator(endNode);}
  const_iterator cend() const {return const_iterator(endNode);}
  iterator end() {return iterator(endNode);}
  iterator begin() {return iterator(beginNode);}

  const_reverse_iterator rbegin() const {return const_reverse_iterator(endNode);}
  const_reverse_iterator crbegin() const {return const_reverse_iterator(endNode);}
  const_reverse_iterator rend() const {return const_reverse_iterator(beginNode);}
  const_reverse_iterator crend() const {return const_reverse_iterator(beginNode);}
  reverse_iterator rend() {return reverse_iterator(beginNode);}
  reverse_iterator rbegin() {return reverse_iterator(endNode);}

  template <typename U>
  void insert(const_iterator thisIterator, U&& value);
  void erase(const_iterator thisIterator) noexcept;
};


template <typename T, typename Allocator>
void List<T, Allocator>::clear() {
  while (listSize > 0) {
    pop_back();
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::copy(const List& another) {
  Node* it = another.begin();
  try {
    while (it != another.end()) {
      std::cout << "ok\n";
      push_back(it->data);
      it = it->next;
    }
  } catch (...) {
    for (auto i = begin(); i != it; ++i) {
      AllocTraitsT::destroy(allocator_, &(static_cast<Node*>(i.ptr_)->data));
    }
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& newAlloc) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator_);
  endNode = all.allocate(1);
  endNode->prev = nullptr;
  endNode->next = nullptr;
  beginNode = endNode;
  allocator_ = newAlloc;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc): List(alloc) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator_);
  endNode = all.allocate(1);
  endNode->prev = nullptr;
  endNode->next = nullptr;
  beginNode = endNode;
  for (size_t i = 0; i < count; ++i) {
    push_back(value);
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc): List(alloc) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator_);
  endNode = all.allocate(1);
  endNode->prev = nullptr;
  endNode->next = nullptr;
  beginNode = endNode;
  for (size_t i = 0; i < count; ++i) {
    push_back();
  }
}


template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other):
    List(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator())) {
  (*this) = other;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(List&& other) {
  *this = std::move(other);
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List<T, Allocator>& other) {
  if (this != &other) {
    clear();
    if(std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value){
      allocator_ = other.get_allocator();
    }
    copy(other);
  }
  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List<T, Allocator>&& other) {
  if (this != &other) {
    beginNode = other.beginNode;
    endNode = other.endNode;
    listSize = other.listSize;
    other.beginNode = nullptr;
    other.endNode = nullptr;
    other.listSize = 0;
  }
  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node>
      all(allocator_);
  clear();
  AllocTraitsT::destroy(allocator_, &static_cast<Node*>(endNode)->data);
  all.deallocate(static_cast<Node*>(endNode), 1);
}

template <typename ElementType, typename Allocator>
void List<ElementType, Allocator>::erase(const_iterator thisIterator) noexcept {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node>
      all(allocator_);
  Node* thisNode = thisIterator.ptr_;
  thisNode->next->prev = thisNode->prev;
  if (thisNode->prev == nullptr) {
    beginNode = thisNode->next;
  } else {
    thisNode->prev->next = thisNode->next;
  }
  --listSize;
  AllocTraitsT::destroy(allocator_, &static_cast<Node*>(thisNode)->data);
  all.deallocate(static_cast<Node*>(thisNode), 1);
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() noexcept {
  erase(begin());
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() noexcept {
  erase(--end());
}

template <typename ElementType, typename Allocator>
template <typename U>
void List<ElementType, Allocator>::insert(const_iterator iterator, U&& value) {
  try {
    typename std::allocator_traits<Allocator>::template rebind_alloc<Node>
        all(allocator_);
    Node* new_node = all.allocate(1);
    AllocTraitsT::construct(allocator_, &new_node->data, std::forward<U>(value));
    Node* thisNode = iterator.ptr_;
    new_node->next = thisNode;
    new_node->prev = thisNode->prev;
    if (thisNode->prev == nullptr) {
      beginNode = new_node;
    } else {
      thisNode->prev->next = new_node;
    }
    thisNode->prev = new_node;
    ++listSize;
  } catch (...) {
    throw;
  }
}

template <typename T, typename Allocator>
template <typename U>
void List<T, Allocator>::push_front(U&& value) {
  try {
    insert(begin(), value);
  } catch (...) {
    throw;
  }
}

template <typename T, typename Allocator>
template <typename U>
void List<T, Allocator>::push_back(U&& value) {
  try {
    insert(end(), value);
  } catch (...) {
    throw;
  }
}

template <typename T, typename Allocator>
Allocator List<T, Allocator>::get_allocator() const noexcept {
  return allocator_;
}

template <typename ElementType, typename Allocator>
template <typename IteratorType>
class List<ElementType, Allocator>::baseIterator {
 public:
  Node* ptr_ = nullptr;

 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = IteratorType;
  using reference = IteratorType&;
  using pointer = IteratorType*;
  using difference_type = std::ptrdiff_t;

  baseIterator(Node* node): ptr_(node) {}
  operator baseIterator<const IteratorType>() {
    return baseIterator<const IteratorType>(ptr_);
  }

  baseIterator& operator++() {
    ptr_ = ptr_->next;
    return *this;
  }

  baseIterator operator++(int) {
    auto another = *this;
    ++(*this);
    return another;
  }

  baseIterator& operator--() {
    ptr_ = ptr_->prev;
    return *this;
  }

  baseIterator operator--(int) {
    auto another = *this;
    --(*this);
    return another;
  }

  bool operator==(const baseIterator& another) const {
    return ptr_ == another.ptr_;
  }

  bool operator!=(const baseIterator& another) const {
    return ptr_ != another.ptr_;
  }

  IteratorType& operator*() const {
    return ptr_->data;
  }

  IteratorType* operator->() const {
    return &ptr_->data;
  }

  baseIterator operator+(int number) const;
  baseIterator operator-(int number) const;

  friend baseIterator operator+(int number, const baseIterator& thisIterator) {
    return thisIterator + number;
  }

  bool is_end() {
    return ptr_ == nullptr || ptr_->next == nullptr;
  }
};

template <typename ElementType, typename Allocator>
template <typename IteratorType>
typename List<ElementType, Allocator>::template baseIterator<IteratorType>
List<ElementType, Allocator>::baseIterator<IteratorType>::operator+(int number) const {
  if (number < 0) {
    return *this - (-number);
  }
  Node* iterateNode = ptr_;
  while (number > 0) {
    iterateNode = ptr_->next;
    --number;
  }
  return iterateNode;
}

template <typename ElementType, typename Allocator>
template <typename IteratorType>
typename List<ElementType, Allocator>::template baseIterator<IteratorType>
List<ElementType, Allocator>::baseIterator<IteratorType>::operator-(int number) const {
  if (number < 0) {
    return *this + (-number);
  }
  Node* iterateNode = ptr_;
  while (number > 0) {
    iterateNode = ptr_->prev;
    --number;
  }
  return iterateNode;
}




template <typename Key, typename Value,
    typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  using NodeType = std::pair<const Key, Value>;
 private:
  typename std::allocator_traits<Allocator>::template rebind_alloc<NodeType> alloc;
  using AllocTraitsT = std::allocator_traits<Allocator>;

  using alloc_for_list_nodes_ = typename std::allocator_traits<Allocator>::
  template rebind_alloc<NodeType*>;
  using alloc_for_iterator = typename std::allocator_traits<Allocator>::
  template rebind_alloc<typename List<NodeType*, alloc_for_list_nodes_>::iterator>;

  using list_iterator = typename List<NodeType*, alloc_for_list_nodes_>::iterator;
  using const_list_iterator = typename List<NodeType*, alloc_for_list_nodes_>::const_iterator;

  Hash hash_function_;
  Equal equal_function_;
  Allocator allocator_;

  List<NodeType*, alloc_for_list_nodes_> nodes_;
  std::vector<list_iterator, alloc_for_iterator> pointers_to_beginnings_;
  std::vector<list_iterator, alloc_for_iterator> pointers_to_ends_;
  size_t blocks_ = 0;
  float max_load_factor_ = 0.8;
  size_t last_hash_node_ = 0;

  void map_to_default();

  template <typename IteratorType>
  class base_iterator;

 public:
  using const_iterator = base_iterator<const NodeType>;
  using iterator = base_iterator<NodeType>;

  UnorderedMap();
  UnorderedMap(const UnorderedMap& other);
  UnorderedMap(UnorderedMap&& other);

  UnorderedMap& operator=(const UnorderedMap & other);
  UnorderedMap& operator=(UnorderedMap&& other) noexcept;

  ~UnorderedMap() {
    for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
      std::allocator_traits<Allocator>::destroy(allocator_, (*it));
      std::allocator_traits<Allocator>::deallocate(allocator_, (*it), 1);
    }
  }

  Value& operator[](const Key& key) {
    auto result = insert(std::make_pair(key, Value()));
    return (result.first)->second;
  }

  Value& operator[](Key&& key) {
    auto result = insert(std::make_pair(std::move(key), Value()));
    return (result.first)->second;
  }

  Value& at(const Key& key);
  const Value& at(const Key& key) const;

  [[nodiscard]] size_t size() const {
    return nodes_.size();
  }

  iterator begin() {
    return nodes_.begin();
  }

  const_iterator begin() const {
    return nodes_.cbegin();
  }

  const_iterator cbegin() const {
    return const_iterator(nodes_.cbegin());
  }

  iterator end() {
    return nodes_.end();
  }

  const_iterator end() const {
    return nodes_.cend();
  }

  const_iterator cend() const {
    return const_iterator(nodes_.cend());
  }

  std::pair<iterator, bool> insert(const NodeType& node);
  template <typename U>
  std::pair<iterator, bool> insert(U&& node);

  template <typename InputIterator>
  void insert(InputIterator first, InputIterator last);

  template <typename... Args >
  std::pair<iterator, bool> emplace(Args&&... args);

  iterator erase(const_iterator position);
  iterator erase(const_iterator first, const_iterator last) {
    for (auto it = first; it != last; it = erase(it));
    return deconst_convert(last);
  }

  template <typename U>
  iterator find(U&& key);

  template <typename U>
  const_iterator find(U&& key) const;

  void reserve(size_t count);

  [[nodiscard]] size_t max_size() const {
    return pointers_to_beginnings_.max_size();
  }
  [[nodiscard]] float load_factor() const {
    return static_cast<float>(size()) / blocks_;
  }
  [[nodiscard]] float max_load_factor() const {
    return max_load_factor_;
  }
  void max_load_factor(float new_max_load_factor_) {
    max_load_factor_ = new_max_load_factor_;
    check_rehash();
  }

 private:
  iterator deconst_convert(const_iterator it) {
    try {
      if (it == cend()) {
        return end();
      }
      return find(it->first);
    } catch (...) {
      throw;
    }
  }

  void rehash(size_t count);
  void check_rehash() {
    try {
      if (load_factor() > max_load_factor_) {
        reserve(blocks_ * 2 + 1);
      }
    } catch (...) {
      throw;
    }
  }
  std::pair<iterator, bool> insert_node(NodeType* new_node);
  bool is_iterators_equal(const_iterator map_iter, list_iterator list_iter) {
    if (map_iter == end() && list_iter == nodes_.end()) return true;
    if (map_iter == end() || list_iter == nodes_.end()) return false;
    return equal_function_((*list_iter)->first, map_iter->first);
  }

  template <typename Iterator, typename U>
  std::optional<Iterator> base_find(U&& key, size_t hash_node) const;

  template <typename U>
  iterator sub_insert(U&& key, size_t& hash_node);
};


template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename IteratorType>
class UnorderedMap<Key, Value, Hash, Equal, Alloc>::base_iterator {
 private:
  using list_iterator_type = typename std::conditional_t<std::is_const<IteratorType>::value,
                                                         const_list_iterator, list_iterator>;
  list_iterator_type lst_iterator;

  list_iterator_type get_list_iterator() {
    return lst_iterator;
  }

  bool is_end() {
    return lst_iterator.is_end();
  }
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = IteratorType;
  using reference = IteratorType&;
  using pointer = IteratorType*;
  using difference_type = std::ptrdiff_t;

  base_iterator(list_iterator it): lst_iterator(it) {}
  base_iterator(const_list_iterator it): lst_iterator(it) {}

  operator base_iterator<const IteratorType>() {
    return base_iterator<const IteratorType>(lst_iterator);
  }

  bool operator==(const base_iterator& another) const {
    return lst_iterator == another.lst_iterator;
  }

  bool operator!=(const base_iterator& another) const {
    return !(*this == another);
  }

  base_iterator& operator++() {
    ++lst_iterator;
    return *this;
  }

  base_iterator operator++(int) {
    auto current_it = *this;
    ++lst_iterator;
    return current_it;
  }

  base_iterator& operator--() {
    --lst_iterator;
    return *this;
  }

  base_iterator operator--(int) {
    auto current_it = *this;
    --lst_iterator;
    return current_it;
  }

  IteratorType& operator*() const {
    return *(*lst_iterator);
  }

  IteratorType* operator->() const {
    return (*lst_iterator);
  }

  friend class UnorderedMap;
};

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap() {
  try {
    nodes_ = List<NodeType*, alloc_for_list_nodes_>();
    pointers_to_beginnings_ =
        std::vector<list_iterator, alloc_for_iterator>(1, nodes_.end());
    pointers_to_ends_ =
        std::vector<list_iterator, alloc_for_iterator>(1, nodes_.end());
    blocks_ = 1;
    max_load_factor_ = 0.8;
    last_hash_node_ = 0;
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap& other) {
  try {
    nodes_ = List<NodeType*, alloc_for_list_nodes_>();
    pointers_to_beginnings_ = std::vector<list_iterator, alloc_for_iterator>(other.blocks_, nodes_.end());
    pointers_to_ends_ = std::vector<list_iterator, alloc_for_iterator>(other.blocks_, nodes_.end());
    blocks_ = other.blocks_;
    max_load_factor_ = other.max_load_factor_;
    last_hash_node_ = other.last_hash_node_;
    bool is_ok = std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value;
    if (is_ok) {
      allocator_ = other.allocator_;
    }
    for (auto it = other.cbegin(); it != other.cend(); ++it) {
      insert(*it);
    }
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(UnorderedMap&& other) {
  try {
    allocator_ = std::move(allocator_);
    nodes_ = std::move(other.nodes_);
    pointers_to_beginnings_ = std::move(other.pointers_to_beginnings_);
    pointers_to_ends_ = std::move(other.pointers_to_ends_);
    blocks_ = other.blocks_;
    max_load_factor_ = other.max_load_factor_;
    last_hash_node_ = other.last_hash_node_;
    other.map_to_default();
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(const UnorderedMap& other) {
  bool is_ok = std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value;
  if(is_ok){
    allocator_ = other.node_alloc;
  }
  nodes_ = other.nodes_;
  blocks_ = other.blocks_;
  pointers_to_beginnings_ = std::vector<list_iterator, alloc_for_iterator>(blocks_, nodes_.end());
  pointers_to_ends_ = std::vector<list_iterator, alloc_for_iterator>(blocks_, nodes_.end());
  for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
    insert(*it);
  }
  max_load_factor_ = other.max_load_factor_;
  last_hash_node_ = other.last_hash_node_;
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(UnorderedMap&& other) noexcept {
  allocator_ = std::move(allocator_);
  nodes_ = std::move(other.nodes_);
  pointers_to_beginnings_ = std::move(other.pointers_to_beginnings_);
  pointers_to_ends_ = std::move(other.pointers_to_ends_);
  blocks_ = other.blocks_;
  max_load_factor_ = other.max_load_factor_;
  last_hash_node_ = other.last_hash_node_;
  other.map_to_default();
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::map_to_default() {
  try {
    nodes_ = List<NodeType*, alloc_for_list_nodes_>();
    pointers_to_beginnings_ =
        std::vector<list_iterator, alloc_for_iterator>(1, nodes_.end());
    pointers_to_ends_ =
        std::vector<list_iterator, alloc_for_iterator>(1, nodes_.end());
    blocks_ = 1;
    max_load_factor_ = 0.8;
    last_hash_node_ = 0;
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) {
  auto it = find(key);
  if (it != end()) {
    return it->second;
  }
  throw std::out_of_range("No this key");
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
const Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) const {
  auto it = find(key);
  if (it != cend()) {
    return it->second;
  }
  throw std::out_of_range("No this key");
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(U&& node) {
  try {
    NodeType* new_node = std::allocator_traits<Alloc>::allocate(allocator_, 1);
    std::allocator_traits<Alloc>::construct(allocator_,
                                            new_node,
                                            std::forward<U>(node));

    return insert_node(new_node);
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(const NodeType& node) {
  try {
    NodeType* new_node = std::allocator_traits<Alloc>::allocate(allocator_, 1);
    std::allocator_traits<Alloc>::construct(allocator_, new_node, node);

    return insert_node(new_node);
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::sub_insert(U&& key, size_t& hash_node) {
  try {
    hash_node = hash_function_(key) % blocks_;
    auto opt_iter = base_find<iterator, U>(key, hash_node);
    if (opt_iter) {
      return opt_iter.value();
    }
    return end();
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert_node(NodeType* new_node) {
  try {
    check_rehash();
    size_t hash_node = 0;
    iterator node_iter = sub_insert(new_node->first,  hash_node);
    if (!node_iter.is_end()) {
      return std::make_pair(node_iter, false);
    }

    auto iter = iterator(pointers_to_beginnings_[hash_node]);
    if (iter.is_end()) {
      nodes_.insert(iter.get_list_iterator(), new_node);
      pointers_to_beginnings_[hash_node] = (--iter.get_list_iterator());
      if (size() > 1) {
        pointers_to_ends_[last_hash_node_] = pointers_to_beginnings_[hash_node];
      }
      last_hash_node_ = hash_node;
      return std::make_pair(iterator(pointers_to_beginnings_[hash_node]), true);
    }

    iter = iterator(pointers_to_ends_[hash_node]);
    auto prev = iterator(pointers_to_ends_[hash_node]);
    nodes_.template insert(iter.get_list_iterator(), new_node);
    ++iter;
    return std::make_pair(--prev, true);
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<class... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::emplace(Args&&... args) {
  try {
    NodeType* new_node = std::allocator_traits<Alloc>::allocate(allocator_, 1);
    std::allocator_traits<Alloc>::construct(allocator_,
                                            new_node,
                                            std::forward<Args>(args)...);
    return insert_node(new_node);
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename InputIterator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(InputIterator first, InputIterator last) {
  try {
    for (auto it = first; it != last; ++it) {
      insert(*it);
    }
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(const_iterator position) {
  bool res = ((position == end()) || (position == cend()));
  if (res) {
    return end();
  }
  size_t hash_node = hash_function_(position->first) % blocks_;
  iterator next = ++(deconst_convert(position));
  res = is_iterators_equal(position, pointers_to_beginnings_[hash_node]);
  if (res) {
    res = is_iterators_equal(next, pointers_to_ends_[hash_node]);
    if (res) {
      pointers_to_beginnings_[hash_node] = nodes_.end();
      pointers_to_ends_[hash_node] = nodes_.end();
    } else {
      pointers_to_beginnings_[hash_node] = next.get_list_iterator();
    }
  } else {
    res = (next != end() && is_iterators_equal(next, pointers_to_ends_[hash_node]));
    if (res) {
      iterator prev = deconst_convert(position);
      pointers_to_ends_[hash_node] = prev.get_list_iterator();
    }
  }
  nodes_.erase(position.get_list_iterator());
  return next;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename Iterator, typename U>
std::optional<Iterator> UnorderedMap<Key, Value, Hash, Equal, Alloc>::base_find(U&& key, size_t hash_node) const {
  try {
    auto it = Iterator(pointers_to_beginnings_[hash_node]);
    auto it_end = Iterator(pointers_to_ends_[hash_node]);
    while (it != it_end) {
      if (equal_function_(it->first, key)) {
        return it;
      }
      ++it;
    }
    return std::nullopt;
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(U&& key) {
  try {
    size_t hash_node = hash_function_(key) % blocks_;
    auto opt_iter = base_find<iterator>(key, hash_node);
    if (opt_iter) {
      return opt_iter.value();
    }
    return end();
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <typename U>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(U&& key) const {
  try {
    size_t hash_node = hash_function_(key) % blocks_;
    auto opt_iter = base_find<const_iterator, U>(key, hash_node);
    if (opt_iter) {
      return opt_iter.value();
    }
    return cend();
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::rehash(size_t count) {
  try {
    List<NodeType*, alloc_for_list_nodes_> old_list = std::move(nodes_);
    map_to_default();
    blocks_ = count;
    pointers_to_beginnings_ =
        std::vector<list_iterator, alloc_for_iterator>(blocks_, nodes_.begin());
    pointers_to_ends_ =
        std::vector<list_iterator, alloc_for_iterator>(blocks_, nodes_.end());
    for (auto it = old_list.begin(); it != old_list.end(); ++it) {
      insert_node(*it);
    }
  } catch (...) {
    throw;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::reserve(size_t count) {
  try {
    if (count > size()) {
      rehash(std::ceil(static_cast<float>(count) / max_load_factor_));
    }
  } catch (...) {
    throw;
  }
}
