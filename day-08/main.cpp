#include <algorithm>
#include <iostream>
#include <numeric>
#include <queue>
#include <vector>

constexpr auto part_1_num_connections = 1000;
constexpr auto part_1_top_k_circuits = 3;
constexpr auto part_2_num_circuits = 1;

template <typename T> struct JBox {
  T x;
  T y;
  T z;

  // output operator for debugging
  friend std::ostream &operator<<(std::ostream &os, const JBox<T> &coord) {
    os << "(" << coord.x << ", " << coord.y << ", " << coord.z << ")";
    return os;
  }

  // input operator for parsing
  friend std::istream &operator>>(std::istream &is, JBox<T> &coord) {
    constexpr auto delimiter = ',';
    char delimiter1, delimiter2;
    is >> coord.x >> delimiter1 >> coord.y >> delimiter2 >> coord.z;
    if (delimiter1 != delimiter || delimiter2 != delimiter) {
      is.setstate(std::ios::failbit);
    }
    return is;
  }

  // Eculidean distance squared to another junction box
  T distance_to(const JBox<T> &other) const {
    return std::pow(other.x - x, 2) + std::pow(other.y - y, 2) +
           std::pow(other.z - z, 2);
  }
};

// Disjoint set union-find data structure for connecting junction boxes
template <typename T> class UnionFind {
private:
  std::vector<size_t> parent;
  std::vector<size_t> size;
  size_t num_disjoint_sets;

public:
  explicit UnionFind(size_t size)
      : parent(size), size(size, 1), num_disjoint_sets(size) {
    for (size_t i = 0; i < size; ++i) {
      parent[i] = i;
    }
  }
  size_t find(size_t x) {
    if (parent[x] != x) {
      parent[x] = find(parent[x]);
    }
    return parent[x];
  }
  void join(size_t x, size_t y) {
    auto rootX = find(x), rootY = find(y);
    if (rootX == rootY) {
      return;
    }
    if (size[rootX] < size[rootY]) {
      std::swap(rootX, rootY);
    }
    parent[rootY] = rootX;
    size[rootX] += size[rootY];
    --num_disjoint_sets;
  }

  auto top_k_sizes_multiplied(const size_t k) {
    decltype(size) root_sizes;
    for (size_t i = 0; i < parent.size(); ++i) {
      if (parent[i] == i) {
        root_sizes.push_back(size[i]);
      }
    }
    if (root_sizes.size() < k) {
      throw std::runtime_error("Not enough disjoint sets to get top k sizes");
    }
    std::nth_element(root_sizes.begin(), root_sizes.begin() + k,
                     root_sizes.end(), std::greater<uint>());
    return std::reduce(root_sizes.begin(), root_sizes.begin() + k, 1ul,
                       std::multiplies<uint>());
  }

  auto get_num_disjoint_sets() const { return num_disjoint_sets; }
};

using JBoxInt = JBox<int64_t>;

// Time: O(N^2 * log(K)), where N is the number of junction boxes and K is
// `part_1_num_connections`
// Space: O(K) + O(N) (for union-find)
auto part_1(const std::vector<JBoxInt> &junction_boxes) {
  // find the top `num_connections` closest junction box pairs using a max heap
  using JBoxPair = std::pair<size_t, size_t>;
  std::priority_queue<std::pair<int64_t, JBoxPair>> max_heap;
  for (size_t i = 0; i < junction_boxes.size(); ++i) {
    for (size_t j = i + 1; j < junction_boxes.size(); ++j) {
      const auto dist = junction_boxes[i].distance_to(junction_boxes[j]);
      max_heap.emplace(dist, JBoxPair{i, j});
      if (max_heap.size() > part_1_num_connections) {
        max_heap.pop();
      }
    }
  }
  // connect the top closest junction boxes
  UnionFind<JBoxInt> uf(junction_boxes.size());
  while (!max_heap.empty()) {
    const auto &[_dist, box_pair] = max_heap.top();
    uf.join(box_pair.first, box_pair.second);
    max_heap.pop();
  }
  return uf.top_k_sizes_multiplied(part_1_top_k_circuits);
}

// Time: O(N^2 log(N)), in the worst case we may need to connect all junction
// boxes, but otherwise it's probably just O(N^2) to build the min heap
// Space: O(N^2)
auto part_2(const std::vector<JBoxInt> &junction_boxes) {
  // we don't know how many connections will be needed to fully connect all the
  // boxes, so we use a min heap to keep track of all of them
  using JBoxPair = std::pair<size_t, size_t>;
  std::vector<std::pair<int64_t, JBoxPair>> all_distances;
  all_distances.reserve((junction_boxes.size() * (junction_boxes.size() - 1)) /
                        2);

  for (size_t i = 0; i < junction_boxes.size(); ++i) {
    for (size_t j = i + 1; j < junction_boxes.size(); ++j) {
      const auto dist = junction_boxes[i].distance_to(junction_boxes[j]);
      all_distances.emplace_back(dist, JBoxPair{i, j});
    }
  }

  std::priority_queue<std::pair<int64_t, JBoxPair>,
                      std::vector<std::pair<int64_t, JBoxPair>>,
                      std::greater<std::pair<int64_t, JBoxPair>>>
      min_heap(std::greater<std::pair<int64_t, JBoxPair>>(),
               std::move(all_distances));
  // connect the top closest junction boxes
  UnionFind<JBoxInt> uf(junction_boxes.size());
  while (!min_heap.empty()) {
    const auto &[_, box_pair] = min_heap.top();
    uf.join(box_pair.first, box_pair.second);
    if (uf.get_num_disjoint_sets() == part_2_num_circuits) {
      return junction_boxes[box_pair.first].x *
             junction_boxes[box_pair.second].x;
    }
    min_heap.pop();
  }
  throw std::runtime_error("Could not fully connect all junction boxes");
}

int main() {
  std::vector<JBox<int64_t>> junction_boxes;
  for (JBox<int64_t> coord; std::cin >> coord;) {
    junction_boxes.push_back(coord);
  }
  std::cout << "Number of junction boxes: " << junction_boxes.size()
            << std::endl;
  std::cout << "Part 1: " << part_1(junction_boxes) << std::endl;
  std::cout << "Part 2: " << part_2(junction_boxes) << std::endl;
  return 0;
}
