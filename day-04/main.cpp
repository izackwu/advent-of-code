#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

int part_1(const std::vector<std::vector<bool>> &grid) {
  const int rows = grid.size(), cols = grid[0].size();
  int total = 0;
  const auto access = [&](int r, int c) -> bool {
    if (r < 0 || r >= rows || c < 0 || c >= cols) {
      return false;
    }
    return grid[r][c];
  };
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (!access(r, c)) {
        continue;
      }
      int count = 0;
      for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          count += access(r + dr, c + dc);
        }
      }
      total += (count <= 4);
    }
  }
  return total;
}

// Topological sort in spirit
// Time: O(rows * cols)
// Space: O(rows * cols)
int part_2(const std::vector<std::vector<bool>> &grid) {
  const int rows = grid.size(), cols = grid[0].size();
  // we number grid[i][j] as i * cols + j
  const auto to_id = [&](int r, int c) { return r * cols + c; };
  std::unordered_map<int, std::unordered_set<int>> adjacency_list;
  int total = 0;
  // first, build the graph
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      if (!grid[r][c]) {
        continue;
      }
      for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          if (dr == 0 && dc == 0) {
            continue;
          }
          int nr = r + dr, nc = c + dc;
          if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
            continue;
          }
          if (grid[nr][nc]) {
            adjacency_list[to_id(r, c)].insert(to_id(nr, nc));
            adjacency_list[to_id(nr, nc)].insert(to_id(r, c));
          }
        }
      }
      // if this node has no neighbors, it's isolated and can be removed
      // directly
      if (adjacency_list.find(to_id(r, c)) == adjacency_list.end()) {
        ++total;
      }
    }
  }
  // now we just need to eliminate nodes with degree <= 3 iteratively
  // we start with nodes whose initial degree <= 3
  std::unordered_set<int> to_process;
  for (const auto &[node, neighbors] : adjacency_list) {
    if (neighbors.size() <= 3) {
      to_process.insert(node);
    }
  }
  // roughly BFS
  while (!to_process.empty()) {
    decltype(to_process) next_to_process;
    for (const auto node : to_process) {
      // skip if already removed. This is necessary as we might try to add a
      // node already in `to_process` to `next_to_process`
      if (adjacency_list.find(node) == adjacency_list.end()) {
        continue;
      }
      // remove this node
      for (const auto neighbor : adjacency_list[node]) {
        adjacency_list[neighbor].erase(node);
        if (adjacency_list[neighbor].size() <= 3) {
          next_to_process.insert(neighbor);
        }
      }
      adjacency_list.erase(node);
      ++total;
    }
    std::swap(to_process, next_to_process);
  }
  return total;
}

int main() {
  std::vector<std::vector<bool>> grid;
  for (std::string input; std::cin >> input;) {
    std::vector<bool> row;
    row.reserve(input.size());
    for (const auto c : input) {
      row.emplace_back(c == '@');
    }
    grid.emplace_back(row);
  }
  // make sure the grid is rectangular
  if (grid.empty() ||
      (std::any_of(grid.cbegin(), grid.cend(), [&](const auto &row) {
        return row.size() != grid[0].size();
      }))) {
    std::cerr << "Invalid input! The grid is not rectangular." << std::endl;
    return 1;
  }
  std::cout << "Grid size: " << grid.size() << " x " << grid.front().size()
            << std::endl;
  std::cout << "Part 1: " << part_1(grid) << std::endl;
  std::cout << "Part 2: " << part_2(grid) << std::endl;
  return 0;
}
