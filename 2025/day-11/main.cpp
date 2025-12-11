#include <cstring>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Node {
  char name[4]; // always 3-letter names
public:
  friend struct std::hash<Node>; // allow std::hash to access private members
  explicit Node(const char *n) {
    std::strncpy(name, n, 3);
    name[3] = '\0';
  }

  explicit Node() : name{""} {}

  bool operator==(const Node &other) const {
    return std::strcmp(name, other.name) == 0;
  }

  friend std::istream &operator>>(std::istream &is, Node &node) {
    // read 3 non-whitespace characters
    is >> node.name[0] >> node.name[1] >> node.name[2];
    node.name[3] = '\0';
    return is;
  }

  friend std::ostream &operator<<(std::ostream &os, const Node &node) {
    os << node.name;
    return os;
  }
};

template <> struct std::hash<Node> {
  auto operator()(const Node &node) const {
    return std::hash<std::string_view>()(std::string_view(node.name, 3));
  }
};

const Node part_1_start_node{"you"}, part_2_start_node{"svr"}, end_node{"out"},
    part_2_fft_node{"fft"}, part_2_dac_node{"dac"};

// Naive solution: just DFS to traverse all paths from "you" to "out".
// This is quite naive as we may visit the same node multiple times and repeat
// work, but it works for the input size.
auto part_1_naive(
    const std::unordered_map<Node, std::unordered_set<Node>> &graph) {
  // there can't be cycles, otherwise there may be infinite paths to "out"
  uint64_t paths_to_out = 0;
  std::vector<Node> to_visit{part_1_start_node};
  while (!to_visit.empty()) {
    const auto current = to_visit.back();
    to_visit.pop_back();
    if (current == end_node) {
      ++paths_to_out;
      continue;
    }
    if (!graph.contains(current)) {
      continue;
    }
    for (const auto &neighbor : graph.at(current)) {
      to_visit.push_back(neighbor);
    }
  }
  return paths_to_out;
}

// DFS with memoization to avoid repeated work.
// Time: O(V + E)
// Space: O(V) for memo AND recursion stack.
auto part_1_dfs_memo(
    const std::unordered_map<Node, std::unordered_set<Node>> &graph) {
  // memoization map: node -> number of paths from node to "out"
  std::unordered_map<Node, uint64_t> memo;

  std::function<uint64_t(const Node &)> dfs =
      [&](const Node &node) -> uint64_t {
    if (node == end_node) {
      return 1u;
    }
    if (memo.contains(node)) {
      return memo.at(node);
    }
    uint64_t total_paths = 0;
    if (graph.contains(node)) {
      for (const auto &neighbor : graph.at(node)) {
        total_paths += dfs(neighbor);
      }
    }
    memo[node] = total_paths;
    return total_paths;
  };
  return dfs(part_1_start_node);
}

auto topological_sort(
    const std::unordered_map<Node, std::unordered_set<Node>> &graph) {
  std::unordered_map<Node, size_t> in_degrees;
  for (const auto &[from, neighbors] : graph) {
    in_degrees.try_emplace(from, 0);
    for (const auto &to : neighbors) {
      ++in_degrees[to];
    }
  }
  std::queue<Node> free_nodes{};
  for (const auto &[node, degree] : in_degrees) {
    if (degree == 0) {
      free_nodes.push(node);
    }
  }
  std::vector<Node> topological_order;
  topological_order.reserve(in_degrees.size());
  while (!free_nodes.empty()) {
    const auto node = free_nodes.front();
    free_nodes.pop();
    topological_order.push_back(node);
    if (!graph.contains(node)) {
      continue;
    }
    for (const auto &neighbor : graph.at(node)) {
      --in_degrees[neighbor];
      if (in_degrees[neighbor] == 0) {
        free_nodes.push(neighbor);
      }
    }
  }
  return topological_order;
}

auto count_paths_between(
    const std::unordered_map<Node, std::unordered_set<Node>> &graph,
    const std::vector<Node> &topological_order, const Node &start,
    const Node &end) {
  std::unordered_map<Node, uint64_t> path_counts;
  path_counts[start] = 1u;
  for (const auto &node : topological_order) {
    if (node == end) { // early termination
      break;
    }
    if (graph.contains(node)) {
      for (const auto &neighbor : graph.at(node)) {
        path_counts[neighbor] += path_counts[node];
      }
    }
  }
  return path_counts[end];
}

// Topological sort + DP solution.
auto part_1_topological_sort(
    const std::unordered_map<Node, std::unordered_set<Node>> &graph) {
  const auto topological_order = topological_sort(graph);
  return count_paths_between(graph, topological_order, part_1_start_node,
                             end_node);
}

auto part_2(const std::unordered_map<Node, std::unordered_set<Node>> &graph) {
  const auto topological_order = topological_sort(graph);
  // Check whether fft or dac comes first - there can't be both a path from
  // fft to dac and a path from dac to fft, otherwise there would be a cycle.
  auto first_node = part_2_fft_node, second_node = part_2_dac_node;
  if (std::find(topological_order.begin(), topological_order.end(),
                first_node) > std::find(topological_order.begin(),
                                        topological_order.end(), second_node)) {
    std::swap(first_node, second_node);
  }
  return count_paths_between(graph, topological_order, part_2_start_node,
                             first_node) *
         count_paths_between(graph, topological_order, first_node,
                             second_node) *
         count_paths_between(graph, topological_order, second_node, end_node);
}

int main() {
  std::unordered_map<Node, std::unordered_set<Node>> graph;
  for (std::string line; std::getline(std::cin, line);) {
    std::istringstream iss(line);
    Node from;
    iss >> from;
    char separator;
    iss >> separator;
    if (separator != ':') {
      throw std::runtime_error("Expected ':' but got" +
                               std::string(1, separator));
    }
    std::unordered_set<Node> to_nodes;
    for (Node to; iss >> to;) {
      to_nodes.insert(to);
    }
    graph[from] = to_nodes;
  }
  std::cout << "Number of nodes with outgoing edges: " << graph.size()
            << std::endl;
  const auto num_edges = std::accumulate(
      graph.begin(), graph.end(), 0u,
      [](auto sum, const auto &pair) { return sum + pair.second.size(); });
  std::cout << "Number of edges: " << num_edges << std::endl;
  std::cout << "Part 1: " << part_1_naive(graph) << std::endl;
  std::cout << "Part 1: " << part_1_dfs_memo(graph) << std::endl;
  std::cout << "Part 1: " << part_1_topological_sort(graph) << std::endl;
  std::cout << "Part 2: " << part_2(graph) << std::endl;
  return 0;
}
