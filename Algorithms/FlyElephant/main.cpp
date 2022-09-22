#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
 
int INF = 1'000'000'000;
int n;
int m;
std::string begin;
std::string end;
int from;
int to;
 
std::vector<std::string> words;
std::unordered_map<std::string, int> un_map;
std::vector<std::vector<std::pair<int, std::string>>> graph;
 
int heur(std::string& s1, std::string& s2) {
  int ans = 0;
  for (int i = 0; i < s1.size(); ++i) {
    if (s1[i] != s2[i]) {
      ans += 1;
    }
  }
  return ans;
}
 
void refresh_vertex(std::vector<int>& used,
                    std::set<std::pair<int, int>>& heap,
                    std::vector<int>& heur_vec,
                    int& check_number) {
  for (int i = 0; i < graph[check_number].size(); ++i) {
    std::pair<int, std::string> par = graph[check_number][i];
    bool b = (used[check_number] + 1 < used[par.first]);
    if (b) {
      used[par.first] = used[check_number] + 1;
      heap.erase({heur_vec[par.first], par.first});
      heur_vec[par.first] = used[par.first] + heur(par.second, end);
      heap.insert({heur_vec[par.first], par.first});
    }
  }
}
 
void a_star() {
  std::set<std::pair<int, int>> heap;
  std::vector<int> used(n, INF);
  used[from] = 0;
  heap.insert(std::make_pair(heur(begin, end), from));
  std::vector<int> heur_vec(n);
  heur_vec[from] = heur(begin, end);
  while (!heap.empty()) {
    int check_number = heap.begin()->second;
    heap.erase(heap.begin());
    if (check_number == to) {
      std::cout << used[check_number] << '\n';
      return;
    }
    refresh_vertex(used, heap, heur_vec, check_number);
  }
  std::cout << "-1\n";
}
 
void create_edges() {
  for (int i = 0; i < n; ++i) {
    std::string str = words[i];
    for (int j = 0; j < str.size(); ++j) {
      char c = str[j];
      for (int k = 0; k < 26; ++k) {
        if (k != (static_cast<int>(c) - 97)) {
          str[j] = static_cast<char>(97 + k);
          bool res = ((un_map.find(str) != un_map.end()));
          if (res) {
            graph[i].emplace_back(un_map[str], str);
          }
        }
      }
      str[j] = c;
    }
  }
}
 
int main() {
  std::ios_base::sync_with_stdio(0);
  std::cin.tie(0);
  std::cout.tie(0);
  std::cin >> n;
  graph.resize(n);
  for (int i = 0; i < n; ++i) {
    std::string elem;
    std::cin >> elem;
    words.push_back(elem);
    un_map[elem] = i;
  }
 
  create_edges();
 
  std::cin >> m;
  for (int i = 0; i < m; ++i) {
    std::cin >> begin;
    std::cin >> end;
    from = un_map[begin];
    to = un_map[end];
    a_star();
  }
}
