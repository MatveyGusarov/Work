#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>
 
template <class Vertex, class Graph, class Visitor>
void BreadthFirstSearch(Vertex origin_vertex, const Graph& graph, Visitor& visitor) {
  std::queue<Vertex> bfs_queue;
  std::unordered_set<Vertex> visited_vertices;
 
  bfs_queue.push(origin_vertex);
  visited_vertices.insert(origin_vertex);
 
  while (!bfs_queue.empty()) {
    auto cur_vertex = bfs_queue.front();
    bfs_queue.pop();
    for (auto& edge : GetOutgoingEdges(graph, cur_vertex)) {
      auto neighbour = GetTarget(graph, edge);
      if (visited_vertices.find(neighbour) == visited_vertices.end()) {
        visitor.TreeEdge(edge);
        bfs_queue.push(neighbour);
        visited_vertices.insert(neighbour);
      }
    }
  }
}
 
template <class Vertex, class Edge>
class BfsVisitor {
 public:
  virtual void TreeEdge(Edge /*edge*/) {}
  virtual ~BfsVisitor() = default;
};
 
class Graph {
 public:
  struct Edge {
    bool operator<(const Edge& other) const {
      if (from != other.from) {
        return from < other.from;
      }
      return to < other.to;
    }
    int from;
    int to;
    int id;
  };
 
  Graph() { }
 
  struct MyEdge {
    int64_t from, to;
    int64_t flow, cap, cost;
    int64_t reversed = -1;
 
    MyEdge(int64_t from, int64_t to, int64_t f, int64_t cap, int64_t cost) : from(from), to(to), flow(f),
                                                                             cap(cap),
                                                                             cost(cost) {}
  };
 
  void addEdge(int64_t from, int64_t to, int64_t flow, int64_t cap, int64_t cost) {
    MyEdge edge = MyEdge(from, to, flow, cap, cost);
    MyEdge rev = MyEdge(to, from, flow, 0, -cost);
 
    myedges_list_[from].push_back(edge);
    myedges_list_[to].push_back(rev);
 
    myedges_list_[from].back().reversed = myedges_list_[to].size() - 1;
    myedges_list_[to].back().reversed = myedges_list_[from].size() - 1;
  }
 
  void SetIdentList(int64_t pos, int64_t value) {
    identificator_list_[pos] = value;
  }
 
  int64_t GetMaxVert() {
    return number_vertices_;
  }
 
  void Levit() {
    std::deque<int64_t> deq;
    deq.push_back(1);
 
    while (!deq.empty()) {
      int64_t u = deq.front();
      deq.pop_front();
      identificator_list_[u] = 2;
 
      for (int64_t i = 0; i < myedges_list_[u].size(); ++i) {
        MyEdge& edge = myedges_list_[u][i];
        bool first_equipment = edge.flow < edge.cap;
        bool second_equipment = distance_list_[edge.to] >
            distance_list_[edge.from] + edge.cost;
        if (first_equipment && second_equipment) {
          distance_list_[edge.to] = distance_list_[edge.from] + edge.cost;
 
          if (identificator_list_[edge.to] == 0) {
            deq.push_back(edge.to);
          } else if (identificator_list_[edge.to] == 2) {
            deq.push_front(edge.to);
          }
 
          identificator_list_[edge.to] = 1;
          parents_list_[edge.to] = &edge;
        }
      }
    }
  }
 
  void MinInReverseNet(int64_t& delta) {
    int64_t vert = GetMaxVert();
    while (vert != 1) {
      MyEdge* edge = parents_list_[vert];
 
      delta = std::min(delta, edge->cap - edge->flow);
 
      vert = parents_list_[vert]->from;
    }
  }
 
  void RelaxReverseWay(int64_t& del, int64_t& minCost) {
    int64_t vert = GetMaxVert();
    while (vert != 1) {
      MyEdge* edge = parents_list_[vert];
      MyEdge* reversed = &myedges_list_[edge->to][edge->reversed];
 
      edge->flow += del;
      reversed->flow -= del;
 
      minCost += del * edge->cost;
      vert = parents_list_[vert]->from;
    }
  }
 
  int64_t GetMinCost() {
    int64_t minCost = 0;
    int64_t maxFlow = 0;
    while (true) {
      identificator_list_.assign(number_vertices_ + 1, 0);
      distance_list_.assign(number_vertices_ + 1, INF_);
      parents_list_.resize(number_vertices_ + 1);
 
      distance_list_[1] = 0;
 
      Levit();
 
      bool is_path = distance_list_[number_vertices_] == INF_;
      if (is_path) {  //  no path
        break;
      } else {
        int64_t delta = INF_;
        MinInReverseNet(delta);
 
        RelaxReverseWay(delta, minCost);
 
        maxFlow += delta;
      }
    }
 
    return minCost;
  }
 
  void ReadGraph() {
    std::cin >> number_vertices_ >> number_edges_;
 
    myedges_list_.assign(number_vertices_ + 1, std::vector<MyEdge>());
    for (int64_t i = 0; i < number_edges_; ++i) {
      int64_t from, to;
      int64_t cap, cost;
      std::cin >> from >> to >> cap >> cost;
 
      addEdge(from, to, 0, cap, cost);
    }
  }
 
  explicit Graph(int number_vertices) : number_vertices_(number_vertices),
                                        adjacency_list_(number_vertices, std::vector<Edge>()) {}
 
  void AddEdge(int from, int to) {
    Edge edge = {from, to, number_edges_};
    adjacency_list_[from].push_back(edge);
    edges_set_.insert(edge);
    ++number_edges_;
  }
 
  void AddVertex() {
    adjacency_list_.emplace_back();
    ++number_vertices_;
  }
 
  int GetNumberVertices() const {
    return number_vertices_;
  }
 
  int GetNumberEdges() const {
    return number_edges_;
  }
 
  std::vector<Edge> GetEdgesFromVertex(int vertex) const {
    return adjacency_list_[vertex];
  }
 
  void BuildEdgesList() {
    edges_list_.clear();
    edges_list_.reserve(number_edges_);
    for (int cur_vertex = 0; cur_vertex < GetNumberVertices(); ++cur_vertex) {
      auto outgoing_edges = GetEdgesFromVertex(cur_vertex);
      for (const auto& edge : outgoing_edges) {
        edges_list_.push_back(edge);
      }
    }
    std::sort(edges_list_.begin(), edges_list_.end(), [](const Edge& lhs, const Edge& rhs) {
      return lhs.id < rhs.id;
    });
  }
 
  std::vector<Edge> GetEdges() const {
    return edges_list_;
  }
 
  int GetEdgeId(int from, int to) const {
    return edges_set_.find({from, to, -1})->id;
  }
 
  Edge GetEdgeById(int id) const {
    return edges_list_[id];
  }
 
 private:
  int64_t INF_ = 1'000'000'000'000'000'000;
 
  int number_edges_ = 0;
  int number_vertices_;
  std::set<Edge> edges_set_;
  std::vector<Edge> edges_list_;
  std::vector<std::vector<Edge>> adjacency_list_;
  std::vector<std::vector<MyEdge>> myedges_list_;
  std::vector<int64_t> distance_list_;
  std::vector<int64_t> identificator_list_;
  std::vector<MyEdge*> parents_list_;
};
 
std::vector<typename Graph::Edge> GetOutgoingEdges(const Graph& graph, int vertex) {
  return graph.GetEdgesFromVertex(vertex);
}
 
int GetTarget(const Graph& /*graph*/, const Graph::Edge& edge) {
  return edge.to;
}
 
class FilteredGraph {
 public:
  FilteredGraph(Graph* graph,
                std::function<bool(Graph::Edge)> predicate) : graph_(graph),
                                                              predicate_(std::move(predicate)) {}
 
  std::vector<Graph::Edge> GetEdgesFromVertex(int vertex) const {
    auto outgoing_edges = graph_->GetEdgesFromVertex(vertex);
    std::vector<Graph::Edge> filtered_edges;
    for (const auto& edge : outgoing_edges) {
      if (predicate_(edge)) {
        filtered_edges.push_back(edge);
      }
    }
    return filtered_edges;
  }
 
 private:
  Graph* graph_;
  std::function<bool(Graph::Edge)> predicate_;
};
 
std::vector<typename Graph::Edge> GetOutgoingEdges(const FilteredGraph& graph, int vertex) {
  return graph.GetEdgesFromVertex(vertex);
}
 
int GetTarget(const FilteredGraph& /*graph*/, const Graph::Edge& edge) {
  return edge.to;
}
 
class FlowNetwork {
 public:
  explicit FlowNetwork(Graph graph) : graph_(std::move(graph)) {}
 
  void UpdateFlowByEdge(const Graph::Edge& edge, int extra_flow) {
    edges_properties_[edge.id].flow += extra_flow;
    edges_properties_[edge.id].residual_capacity = edges_properties_[edge.id].capacity -
        edges_properties_[edge.id].flow;
    int rev_edge_id = edges_properties_[edge.id].reverse_edge_id;
    edges_properties_[rev_edge_id].flow -= extra_flow;
    edges_properties_[rev_edge_id].residual_capacity = edges_properties_[rev_edge_id].capacity -
        edges_properties_[rev_edge_id].flow;
  }
 
  int GetResidualCapacity(int edge_id) const {
    return edges_properties_[edge_id].residual_capacity;
  }
 
  int GetNumberEdges() const {
    return graph_.GetNumberEdges();
  }
 
  int GetNumberVertices() const {
    return graph_.GetNumberVertices();
  }
 
  int GetSource() const {
    return source_;
  }
 
  int GetSink() const {
    return sink_;
  }
 
  int GetEdgeId(int from, int to) const {
    return graph_.GetEdgeId(from, to);
  }
 
  Graph::Edge GetEdgeById(int id) const {
    return graph_.GetEdgeById(id);
  }
 
  FilteredGraph ResidualNetworkView() {
    return FilteredGraph(&graph_, [this](Graph::Edge edge) {
      return edges_properties_[edge.id].residual_capacity > 0;
    });
  }
 
  friend class NetworkBuilder;
 
 private:
  struct EdgeProperties {
    int flow;
    int capacity;
    int residual_capacity;
    int reverse_edge_id;
  };
  int source_;
  int sink_;
  std::vector<EdgeProperties> edges_properties_;
  Graph graph_;
};
 
class NetworkBuilder {
 public:
  static std::unique_ptr<FlowNetwork> Build(const Graph& graph,
                                            const std::vector<int>& gold_capacity,
                                            int gold_limit) {
    auto network = std::make_unique<FlowNetwork>(graph);
    AssignSourceSink(network.get());
    AddEdgesFromSource(network.get());
    AddEdgesToSink(network.get());
    network->graph_.BuildEdgesList();
    AddReverseEdges(network.get());
    network->graph_.BuildEdgesList();
    GenerateEdgeProperties(network.get(), gold_capacity, gold_limit);
    return network;
  }
 
 private:
  static constexpr int kInf = 2e9;
  static void AssignSourceSink(FlowNetwork* network) {
    int number_vertices = network->graph_.GetNumberVertices();
    network->graph_.AddVertex();
    network->graph_.AddVertex();
    network->source_ = number_vertices;
    network->sink_ = number_vertices + 1;
  }
 
  static void AddEdgesFromSource(FlowNetwork* network) {
    for (int cur_person = 0; cur_person < network->graph_.GetNumberVertices() - 2; ++cur_person) {
      network->graph_.AddEdge(network->source_, cur_person);
    }
  }
 
  static void AddEdgesToSink(FlowNetwork* network) {
    for (int cur_person = 0; cur_person < network->graph_.GetNumberVertices() - 2; ++cur_person) {
      network->graph_.AddEdge(cur_person, network->sink_);
    }
  }
 
  static void AddReverseEdges(FlowNetwork* network) {
    auto edges = network->graph_.GetEdges();
    for (const auto& edge : edges) {
      network->graph_.AddEdge(edge.to, edge.from);
    }
  }
 
  static void GenerateEdgeProperties(FlowNetwork* network, const std::vector<int>& gold_capacity,
                                     int gold_limit) {
    network->edges_properties_.resize(network->graph_.GetNumberEdges());
    auto edges = network->graph_.GetEdges();
    for (const auto& edge : edges) {
      int reversed_edge_id;
      if (edge.id < network->graph_.GetNumberEdges() / 2) {
        reversed_edge_id = network->graph_.GetNumberEdges() / 2 + edge.id;
      } else {
        reversed_edge_id = edge.id - network->graph_.GetNumberEdges() / 2;
      }
      if (edge.from == network->source_) {
        network->edges_properties_[edge.id] = {0, gold_capacity[edge.to],
                                               gold_capacity[edge.to], reversed_edge_id};
      } else if (edge.to == network->sink_) {
        network->edges_properties_[edge.id] = {0, gold_limit, gold_limit, reversed_edge_id};
      } else if (edge.id < network->graph_.GetNumberEdges() / 2) {
        network->edges_properties_[edge.id] = {0, kInf, kInf, reversed_edge_id};
      } else {
        network->edges_properties_[edge.id] = {0, 0, 0, reversed_edge_id};
      }
    }
  }
};
 
class AncestorsCalculator : public BfsVisitor<int, Graph::Edge> {
 public:
  explicit AncestorsCalculator(std::vector<int>* ancestors) : ancestors_(ancestors) {}
 
  void TreeEdge(Graph::Edge edge) override {
    (*ancestors_)[edge.to] = edge.from;
  }
 
 private:
  std::vector<int>* ancestors_;
};
 
class EdmondsKarpAlgorithm {
 public:
  explicit EdmondsKarpAlgorithm(FlowNetwork network) : network_(std::move(network)) {}
 
  int FindMaxFlow() {
    int max_flow = 0;
    bool is_sink_reachable = true;
    while (is_sink_reachable) {
      auto ancestors = GetAncestorsInTree();
      if (ancestors[network_.GetSink()] == -1) {
        is_sink_reachable = false;
        continue;
      }
      auto edges_id_on_path = GetPathEdgesId(ancestors);
      int extra_flow = GetExtraFlow(edges_id_on_path);
      for (int id : edges_id_on_path) {
        network_.UpdateFlowByEdge(network_.GetEdgeById(id), extra_flow);
      }
      max_flow += extra_flow;
    }
    return max_flow;
  }
 
 private:
  std::vector<int> GetAncestorsInTree() {
    std::vector<int> ancestors(network_.GetNumberVertices(), -1);
    AncestorsCalculator ancestors_calculator(&ancestors);
    auto residual_network = network_.ResidualNetworkView();
    BreadthFirstSearch(network_.GetSource(), residual_network, ancestors_calculator);
    return ancestors;
  }
 
  std::vector<int> GetPathEdgesId(const std::vector<int>& ancestors) {
    std::vector<int> edges_id;
    int cur_vertex = network_.GetSink();
    while (cur_vertex != network_.GetSource()) {
      int ancestor = ancestors[cur_vertex];
      int edge_id = network_.GetEdgeId(ancestor, cur_vertex);
      edges_id.push_back(edge_id);
      cur_vertex = ancestor;
    }
    std::reverse(edges_id.begin(), edges_id.end());
    return edges_id;
  }
 
  int GetExtraFlow(const std::vector<int>& edges_id_on_path) {
    int min_capacity = static_cast<int>(2e9);
    for (int id : edges_id_on_path) {
      min_capacity = std::min(min_capacity, network_.GetResidualCapacity(id));
    }
    return min_capacity;
  }
 
  FlowNetwork network_;
};
 
/**
 * \brief Функция поиска минимального значения из диапазона, для которого верен бинарный предикат, где один из аргументов фиксирован
 * \tparam Iterator - итератор, который удовлетворяет требованиям на ForwardIterator
 * \tparam T - тип значения, с которым будет вычисление в предикате
 * \tparam BinaryPredicate - тип бинарного предиката, который на основе двух объектов типа T возвращает bool
 * \param from - начало диапазона, в котором осуществляется поиск
 * \param to - конец диапазона, в котором осуществляется поиск (указывает за последний элемент)
 * \param value - фиксированное значение, с которым происходит вычисление предиката
 * \param predicate - сам предикат
 * \returns Итератор на искомый элемент диапазона при его наличии или на end при отсутствии такового
 * \warning Диапазон поиска должен быть предварительно отсортирован, иначе результат не определен
 * \par Сложность: число вызовов предиката составляет log_2(to - from) + O(1), но, если итератор не удовлетворяет требованию RandomAccess, то число инкрементов линейное
 */
template<class Iterator, class T, class BinaryPredicate>
Iterator LowerBound(Iterator from, Iterator to, const T& value, BinaryPredicate predicate) {
  auto distance = std::distance(from, to);
  Iterator mid;
  while (distance > 0) {
    mid = from;
    std::advance(mid, distance / 2);
    if (predicate(*mid, value)) {
      from = ++mid;
      distance -= (distance / 2 + 1);
    } else {
      distance /= 2;
    }
  }
  return mid;
}
 
template<typename T>
std::vector<T> ReadVector(int number_elements) {
  std::vector<T> result(number_elements);
  for (auto& elem : result) {
    std::cin >> elem;
  }
  return result;
}
 
void ReadGraph(Graph& graph, int number_edges, std::istream& is = std::cin) {
  int from, to;
  for (int i = 0; i < number_edges; ++i) {
    is >> from >> to;
    graph.AddEdge(--from, --to);
  }
}
 
template<typename T>
std::vector<T> Range(T from, T to) {
  std::vector<T> range;
  while (from < to) {
    range.push_back(from);
    ++from;
  }
  return range;
}
 
int main() {
  Graph graph;
 
  graph.ReadGraph();
 
  std::cout << graph.GetMinCost();
}
