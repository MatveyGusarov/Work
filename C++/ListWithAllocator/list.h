#include <iostream>
#include <memory>
#include <cstddef>

template<size_t N>
class StackStorage {
 public:
  StackStorage() {}

  StackStorage(const StackStorage &) = delete;

  StackStorage &operator=(const StackStorage &) = delete;

  uint8_t *allocate(size_t bytes, size_t alignmentSize) {
    cur += ((alignmentSize - (cur % alignmentSize)) % alignmentSize);
    uint8_t *posToAllocate = storage + cur;
    cur += bytes;
    if (cur > N) {
      cur -= bytes;
      throw std::bad_alloc();
    }
    return posToAllocate;
  }

 private:
  size_t cur = 0;
  alignas(std::max_align_t) uint8_t storage[N];
};

template<typename T, size_t N>
class StackAllocator {
 public:
  explicit StackAllocator(StackStorage<N>& arr) : storage(&arr) {}
  StackAllocator() {};

  ~StackAllocator() = default;

  template<typename U>
  StackAllocator(const StackAllocator<U, N>& alloc): storage(alloc.GetStorage()) {}
  template<typename U>
  StackAllocator& operator=(const StackAllocator<U, N>& alloc) {
    storage = alloc.GetStorage();
    return *this;
  }

  T* allocate(size_t n) {
    return reinterpret_cast<T*>(storage->allocate(n * sizeof(T), alignof(T)));
  }

  void deallocate(T*, size_t) {}

  template<class U>
  struct rebind {
    typedef StackAllocator<U, N> other;
  };

  StackStorage<N>* GetStorage() const {
    return storage;
  }

  static StackAllocator<T, N> select_on_container_copy_construction(const StackAllocator<T, N>& alloc) {
    return alloc;
  }

  template<typename U>
  bool operator==(const StackAllocator<U, N>& alloc2) const {
    return storage == alloc2.storage;
  }

  using value_type = T;
  using propagate_on_container_copy_assignment = std::true_type;
  using is_always_equal = std::true_type;
 private:
  StackStorage<N>* storage;
};

template<typename T, typename U, size_t N>
bool operator!=(const StackAllocator<T, N>& alloc1, const StackAllocator<U, N>& alloc2) {
  return !(alloc1 == alloc2);
}



template<typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct NodeBase;
  struct Node;

  template<bool IsConst>
  class ListIterator;

  struct NodeBase {
    NodeBase(NodeBase* left, NodeBase* right) : prev(left), next(right) {}
    NodeBase* prev;
    NodeBase* next;
  };

  struct Node : public NodeBase {
    Node(NodeBase* left, NodeBase* right, const T& dat) : NodeBase(left, right), data(dat) { }

    T data;
  };

  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc;
  using AllocTraitsT = std::allocator_traits<Allocator>;

  size_t list_size;
  Allocator allocator;
  NodeBase* head;
  NodeBase* tail;

 public:
  using value_type = T;
  using iterator = ListIterator<false>;
  using const_iterator = ListIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List();
  explicit List(size_t count, const T& value);
  ~List() {
    int64_t sz = list_size;
    for (int64_t i = 0; i < sz; ++i) {
      auto it = begin();
      erase(it);
    }
  }

  List(const List<T, Allocator>& other);
  List(Allocator& stack_allocator);
  List(int64_t num, Allocator& allocator);
  explicit List(size_t num);

  List& operator=(const List<T, Allocator>& other);

  size_t size() const;
  Allocator get_allocator() const {
    return allocator;
  }

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

  iterator begin() { return iterator(head->next); }
  const_iterator begin() const { return const_iterator(head->next); }

  iterator end() { return iterator(tail); }
  const_iterator end() const { return const_iterator(tail); }

  reverse_iterator rbegin() { return std::reverse_iterator(tail); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(tail); }

  reverse_iterator rend() { return reverse_iterator(head); }
  const_reverse_iterator rend() const { return const_reverse_iterator(head); }

  const_iterator cbegin() const { return const_iterator(head->next); }
  const_iterator cend() const { return const_iterator(tail); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator(tail->prev); }
  const_reverse_iterator crend() const { return const_reverse_iterator(head); }

  void push_back(const T& value);
  void push_front(const T& value);

  void pop_back();
  void pop_front();

  template<bool IsConst>
  iterator insert(ListIterator<IsConst> pos, const T& value);

  template<bool IsConst>
  iterator erase(ListIterator<IsConst> pos);
};

template<typename T, typename Allocator>
List<T, Allocator>::List() {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator);
  list_size = 0;
  NodeBase* new_node = all.allocate(1);
  head = new_node;
  new_node = all.allocate(1);
  tail = new_node;
  tail->prev = head;
  head->next = tail;
}

template<typename T, typename StackAllocator>
List<T, StackAllocator>::List(size_t count, const T& value) : List() {
  list_size = count;
  if(count == 0)
    return;

  Node* current_node = alloc.allocate(1);
  AllocTraitsT::construct(allocator, &current_node->data, value);
  head->next = current_node;

  for(size_t i = 1; i < count; ++i) {
    Node* new_node = alloc.allocate(1);
    AllocTraitsT::construct(allocator, &new_node->data, value);
    current_node->next = new_node;
    current_node = new_node;
  }
  current_node->next = tail;
  tail->prev = current_node;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(Allocator& allocator) : allocator(AllocTraitsT::select_on_container_copy_construction(allocator)) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator);
  list_size = 0;
  head = all.allocate(1);
  tail = all.allocate(1);
  tail->prev = head;
  head->next = tail;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(int64_t num, Allocator& allocator) : list_size(num), allocator(AllocTraitsT::select_on_container_copy_construction(allocator)) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator);
  head = all.allocate(1);
  tail = all.allocate(1);
  head->next = tail;
  tail->prev = head;

  auto current = head;
  for (size_t i = 0; i < list_size; ++i) {
    NodeBase* next = all.allocate(1);
    current->next = next;
    next->prev = current;
    current = next;
  }
  current->next = tail;
  tail->prev = current;

  auto it = begin();
  try {
    for (; it != end(); ++it) {
      AllocTraitsT::construct(allocator, &(static_cast<Node*>(it.ptr)->data));
    }
  } catch(...) {
    for (auto i = begin(); i != it; ++i) {
      AllocTraitsT::destroy(allocator, &(static_cast<Node*>(i.ptr)->data));
    }
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List<T, Allocator>& other) : allocator(AllocTraitsT::select_on_container_copy_construction(other.allocator)) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator);
  NodeBase* new_node = all.allocate(1);
  head = new_node;
  new_node = all.allocate(1);
  tail = new_node;
  head->next = tail;
  tail->prev = head;
  list_size = 0;
  auto it = other.begin();
  try {
    for (it = other.begin(); it != other.end(); it++) {
      push_back(*it);
    }
  } catch(...) {
    for (auto i = begin(); i != end(); ++i) {
      AllocTraitsT::destroy(allocator, &(static_cast<Node*>(i.ptr)->data));
    }
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t num) {
  list_size = 0;
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator);
  head = all.allocate(1);
  tail = all.allocate(1);
  head->next = tail;
  tail->prev = head;
  if (num == 0) {
    return;
  }
  try {
    Node* node = all.allocate(1);
    AllocTraitsT::construct(allocator, &node->data);
    list_size += 1;
    head->next = node;
    tail->prev = node;
    node->next = tail;
    node->prev = head;
    for (size_t i = 0; i < num; ++i) {
      Node* current = all.allocate(1);
      AllocTraitsT::construct(allocator, &current->data);
      list_size += 1;
      node->next = current;
      tail->prev = current;
      current->prev = node;
      current->next = tail;
      node = current;
    }
  } catch (...) {
    int64_t sz = list_size;
    for (int64_t i = 0; i < sz; ++i) {
      auto it = begin();
      erase(it);
    }
    throw;
  }
}

template<typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List<T, Allocator>& other) {
  if(this == &other)
    return *this;
  List<T, Allocator> copy(other);
  if (AllocTraitsT::propagate_on_container_copy_assignment::value &&
      allocator != other.allocator) {
    allocator = other.allocator;
  }
  std::swap(head, copy.head);
  std::swap(tail, copy.tail);
  std::swap(list_size, copy.list_size);
  return *this;
}

template<typename T, typename StackAllocator>
size_t List<T, StackAllocator>::size() const {
  return list_size;
}

template<typename T, typename StackAllocator>
T& List<T, StackAllocator>::front() {
  return static_cast<Node*>(head->next)->data;
}

template<typename T, typename StackAllocator>
T& List<T, StackAllocator>::back() {
  return static_cast<Node*>(tail->prev)->data;
}

template<typename T, typename StackAllocator>
const T& List<T, StackAllocator>::front() const {
  return static_cast<Node*>(head->next)->data;
}

template<typename T, typename StackAllocator>
const T& List<T, StackAllocator>::back() const {
  return static_cast<Node*>(tail->prev)->data;
}

template<typename T, typename Allocator>
template<bool IsConst>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(ListIterator<IsConst> pos, const T& value) {
  if (pos == iterator(head))
    ++pos;

  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator);
  Node* new_node = all.allocate(1);
  try {
    AllocTraitsT::construct(allocator, &new_node->data, value);
    pos.ptr->prev->next = new_node;
    new_node->prev = pos.ptr->prev;
    pos.ptr->prev = new_node;
    new_node->next = pos.ptr;

    ++list_size;
    return iterator(new_node);
  } catch(...) {
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& value) {
  auto it = end();
  insert(it, value);
}

template<typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  auto it = begin();
  insert(it, value);
}

template<typename T, typename Allocator>
template<bool IsConst>
typename List<T, Allocator>::iterator List<T, Allocator>::erase(ListIterator<IsConst> pos) {
  typename std::allocator_traits<Allocator>::template rebind_alloc<Node> all(allocator);
  pos.ptr->prev->next = pos.ptr->next;
  pos.ptr->next->prev = pos.ptr->prev;
  Node* node = static_cast<Node*>(pos.ptr->next);
  --list_size;

  AllocTraitsT::destroy(allocator, &static_cast<Node*>(pos.ptr)->data);
  all.deallocate(static_cast<Node*>(pos.ptr), 1);

  return iterator(node);
}

template<typename T, typename StackAllocator>
void List<T, StackAllocator>::pop_front() {
  if(list_size == 0)
    return;

  auto beg = begin();
  erase(beg);
}

template<typename T, typename StackAllocator>
void List<T, StackAllocator>::pop_back() {
  if(list_size == 0)
    return;

  auto it = end();
  erase(--it);
}




template <typename T, typename StackAllocator>
template<bool IsConst>
class List<T, StackAllocator>::ListIterator {
 private:
  NodeBase* ptr;
 public:
  friend class List<T, StackAllocator>;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using value_type = std::conditional_t<IsConst, const T, T>;

  ListIterator(NodeBase* ptr) : ptr(ptr) {}
  ListIterator(const ListIterator<IsConst>& other) : ptr(other.ptr) {}

  operator ListIterator<true>() {
    return ListIterator<true>(ptr);
  };

  reference operator*() { return static_cast<Node*>(ptr)->data; }
  reference operator*() const { return static_cast<Node*>(ptr)->data; }
  pointer operator->() { return &(static_cast<Node*>(ptr)->data); }
  pointer operator->() const { return &(static_cast<Node*>(ptr)->data); }

  ListIterator<IsConst>& operator++() {
    ptr = ptr->next;
    return *this;
  }

  ListIterator<IsConst> operator++(int) {
    ListIterator<IsConst> copy(*this);
    ++(*this);
    return copy;
  }

  ListIterator<IsConst>& operator--() {
    ptr = ptr->prev;
    return *this;
  }

  ListIterator<IsConst> operator--(int) {
    ListIterator<IsConst> copy(*this);
    --(*this);
    return copy;
  }

  T& getNode() {
    return static_cast<Node*>(ptr)->data;
  }

  template<bool IsConstOther>
  bool operator==(const ListIterator<IsConstOther>& other) const {
    return ptr == other.ptr;
  }

  template<bool IsConstOther>
  bool operator!=(const ListIterator<IsConstOther>& other) const {
    return !(*this == other);
  }
};
