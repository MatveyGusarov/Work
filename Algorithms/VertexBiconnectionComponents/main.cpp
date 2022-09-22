#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
 
int num_ver;
int num_edge;
int Time = 0;
std::vector<int> roots;
std::vector<bool> edge_used;
std::vector<std::vector<int>> vert;
std::vector<std::vector<std::vector<int>>> vec;
 
void Input() {
  int from;
  int to;
  for (int i = 0; i < num_edge; ++i) {
    std::cin >> from;
    std::cin >> to;
    std::vector<int> e_from(5);
    e_from[0] = from;
    e_from[1] = to;
    e_from[4] = i;
    std::vector<int> e_to(5);
    e_to[0] = to;
    e_to[1] = from;
    e_to[4] = i;
    vec[from].push_back(e_from);
    vec[to].push_back(e_to);
  }
}
 
void dfs(int v, int p) {
  vert[v][0] = v;
  vert[v][6] = 1;
  vert[v][1] = ++Time;
  vert[v][3] = p;
  vert[v][4] = vert[v][1];
  for (int i = 0; i < vec[v].size(); ++i) {
    std::vector<int>& e = vec[v][i];
    if (p == e[1]) {
      if (!edge_used[e[4]]) {
        vert[v][4] = std::min(vert[v][4], vert[e[1]][1]);
        edge_used[e[4]] = true;
      }
      continue;
    }
 
    if (vert[e[1]][6]) {
      edge_used[e[4]] = true;
      vert[v][4] = std::min(vert[v][4], vert[e[1]][1]);
    } else {
      edge_used[e[4]] = true;
      e[2] = 1;
      if (p == -1) {
        vert[v][5] += 1;
      }
      dfs(e[1], v);
      vert[v][4] = std::min(vert[v][4], vert[e[1]][4]);
    }
  }
  vert[v][2] = ++Time;
}
 
void Initialize() {
  vec.resize(num_ver);
  edge_used.resize(num_edge, false);
  vert.resize(num_ver, std::vector<int>(8));
  Input();
  for (int i = 0; i < num_ver; ++i) {
    if (!vert[i][6]) {
      roots.push_back(i);
      vert[i][7] = 1;
      dfs(i, -1);
    }
  }
}
 
void DfsForBlock(std::vector<std::set<int>>& ans,
                 std::queue<std::vector<int>>& new_blocks, int v, int num_comp) {
  vert[v][6] = true;
  for (int i = 0; i < vec[v].size(); ++i) {
    std::vector<int> e = vec[v][i];
    if (!e[2]) {
      if(e[1] == vert[v][3]) {
        if(!edge_used[e[4]]) {
          edge_used[e[4]] = true;
          ans[num_comp].insert(e[4]);
        }
        continue;
      }
      if (vert[e[1]][4] < vert[v][1]) {
        edge_used[e[4]] = true;
        ans[num_comp].insert(e[4]);
      }
    } else {
      edge_used[e[4]] = true;
      if (vert[e[1]][4] < vert[v][1] || (vert[v][7] && (vert[e[1]][4] == vert[v][1]))) {
        e[3] = 1;
        ans[num_comp].insert(e[4]);
        if (!vert[e[1]][6]) {
          vert[e[1]][6] = true;
          DfsForBlock(ans, new_blocks, e[1], num_comp);
        }
      } else {
        e[3] = 1;
        if(!vert[e[1]][6]) {
          vert[e[1]][6] = 1;
          new_blocks.push(e);
        }
      }
    }
  }
}
 
std::vector<std::set<int>> Solve() {
  edge_used = std::vector<bool>(num_edge, false);
  std::vector<std::set<int>> ans;
  int num_comp = -1;
  for (int i = 0; i < num_ver; ++i) {
    vert[i][6] = 0;
  }
 
  std::queue<std::vector<int>> new_blocks;
  for (int i = 0; i < roots.size(); ++i) {
    int root = roots[i];
    vert[root][6] = 1;
    for (int i = 0; i < vec[root].size(); ++i) {
      std::vector<int> e = vec[root][i];
      if (e[2]) {
        new_blocks.push(e);
      }
    }
  }
 
  std::vector<int> cur_edge;
  int cur;
  while (!new_blocks.empty()) {
    cur_edge = new_blocks.front();
    num_comp += 1;
    ans.emplace_back();
    ans[num_comp].insert(cur_edge[4]);
    cur = cur_edge[1];
    new_blocks.pop();
 
    DfsForBlock(ans, new_blocks, cur, num_comp);
  }
  return ans;
}
 
int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::cin >> num_ver;
  std::cin >> num_edge;
  Initialize();
  std::vector<std::set<int>> ans = Solve();
 
  std::cout << ans.size() << "\n";
  for (int i = 0; i < ans.size(); ++i) {
    std::set<int> block = ans[i];
    std::cout << block.size() << " ";
    for (auto it = block.begin(); it != block.end(); it++) {
      std::cout << *it << " ";
    }
    std::cout << "\n";
  }
}
