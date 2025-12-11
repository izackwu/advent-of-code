#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <queue>
#include <stdexcept>
#include <vector>

template <std::signed_integral T> struct Tile {
  using UnsignedT = std::make_unsigned_t<T>;
  T x;
  T y;

  // output operator for debugging
  friend std::ostream &operator<<(std::ostream &os, const Tile<T> &coord) {
    os << "(" << coord.x << ", " << coord.y << ")";
    return os;
  }

  // input operator for parsing
  friend std::istream &operator>>(std::istream &is, Tile<T> &coord) {
    constexpr auto delimiter = ',';
    char delimiter1;
    is >> coord.x >> delimiter1 >> coord.y;
    if (delimiter1 != delimiter) {
      is.setstate(std::ios::failbit);
    }
    return is;
  }

  UnsignedT rectangle_area_to(const Tile<T> &other) const {
    return (std::abs(other.x - x) + 1) * (std::abs(other.y - y) + 1);
  }
};

auto part_1_naive(const std::vector<Tile<int64_t>> &tiles) {
  uint64_t max_area = 0;
  for (size_t i = 0; i < tiles.size(); ++i) {
    for (size_t j = i + 1; j < tiles.size(); ++j) {
      const auto area = tiles[i].rectangle_area_to(tiles[j]);
      if (area > max_area) {
        max_area = area;
      }
    }
  }
  return max_area;
}

// This class encapsulates the logic to compress a list of coordinate values,
// and retrieve them.
class CompressedValues {
private:
  auto compress(const std::vector<int64_t> &raw_values) {
    std::vector<int64_t> unique_values = raw_values;
    std::sort(unique_values.begin(), unique_values.end());
    auto last = std::unique(unique_values.begin(), unique_values.end());
    unique_values.erase(last, unique_values.end());
    return unique_values;
  }

private:
  const std::vector<int64_t> compressed_values;

public:
  CompressedValues(const std::vector<int64_t> &raw_values)
      : compressed_values(compress(raw_values)) {}

  // Compress a raw value to its 1-based index in the compressed values.
  size_t compress(int64_t value) const {
    auto it = std::lower_bound(compressed_values.begin(),
                               compressed_values.end(), value);
    if (it == compressed_values.end() || *it != value) {
      throw std::out_of_range("Value not found in compressed values.");
    }
    return std::distance(compressed_values.begin(), it) + 1;
  }

  int64_t decompress(size_t index) const {
    if (index > compressed_values.size()) {
      throw std::out_of_range("Index out of range for compressed values.");
    }
    return compressed_values[index - 1];
  }

  size_t size() const { return compressed_values.size(); }
};

// we consider edge tiles to be inside the polygon
enum class CompressedTileType { UNKNOWN, INSIDE, OUTSIDE };

auto part_2(const std::vector<Tile<int64_t>> &tiles) {
  // 1. Collect all x and y values from the tiles, and compress them.
  std::vector<int64_t> x_values, y_values;
  for (const auto &tile : tiles) {
    x_values.push_back(tile.x);
    y_values.push_back(tile.y);
  }
  CompressedValues compressed_x(x_values), compressed_y(y_values);
  // 2. Create a 2D grid using the compressed values
  const auto compressed_width = compressed_x.size(),
             compressed_height = compressed_y.size();
  std::cout << "Compressed grid size: " << compressed_width << " x "
            << compressed_height << std::endl;
  // 2 extra rows and columns as padding, so that we can flood fill the outside
  // area
  std::vector<std::vector<CompressedTileType>> grid(
      compressed_width + 2,
      std::vector<CompressedTileType>(compressed_height + 2,
                                      CompressedTileType::UNKNOWN));
  // 3. Draw the polygon edges on the grid
  const auto n = tiles.size();
  for (size_t i = 0; i < n; ++i) {
    // polygon edge: tiles[i] -> tiles[(i + 1) % n]
    const auto &start = tiles[i], &end = tiles[(i + 1) % n];
    const auto start_x = compressed_x.compress(start.x),
               start_y = compressed_y.compress(start.y),
               end_x = compressed_x.compress(end.x),
               end_y = compressed_y.compress(end.y);
    if (start_x == end_x) {
      // vertical line
      const auto min_y = std::min(start_y, end_y);
      const auto max_y = std::max(start_y, end_y);
      for (size_t y = min_y; y <= max_y; ++y) {
        grid[start_x][y] = CompressedTileType::INSIDE;
      }
    } else if (start_y == end_y) {
      // horizontal line
      const auto min_x = std::min(start_x, end_x);
      const auto max_x = std::max(start_x, end_x);
      for (size_t x = min_x; x <= max_x; ++x) {
        grid[x][start_y] = CompressedTileType::INSIDE;
      }
    } else {
      throw std::runtime_error(
          "Only horizontal and vertical lines are supported.");
    }
  }
  // 4. Flood fill the outside area from (0,0)
  std::queue<std::pair<size_t, size_t>> to_visit;
  to_visit.push({0, 0});
  grid[0][0] = CompressedTileType::OUTSIDE;
  const std::vector<std::pair<int64_t, int64_t>> directions{
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  while (!to_visit.empty()) {
    const auto [x, y] = to_visit.front();
    to_visit.pop();
    for (const auto &[dx, dy] : directions) {
      const auto nx = x + dx;
      const auto ny = y + dy;
      // because we use unsigned indices, we only need to check the upper bounds
      if (nx < compressed_width + 2 && ny < compressed_height + 2 &&
          grid[nx][ny] == CompressedTileType::UNKNOWN) {
        grid[nx][ny] = CompressedTileType::OUTSIDE;
        to_visit.push({nx, ny});
      }
    }
  }

  // DEBUG: print the compressed grid
  // std::cout << "\nCompressed grid:" << std::endl;
  // for (size_t y = 0; y < compressed_height + 2; ++y) {
  //   for (size_t x = 0; x < compressed_width + 2; ++x) {
  //     char c = '?';
  //     switch (grid[x][y]) {
  //     case CompressedTileType::UNKNOWN:
  //       c = '.';
  //       break;
  //     case CompressedTileType::INSIDE:
  //       c = '#';
  //       break;
  //     case CompressedTileType::OUTSIDE:
  //       c = '@';
  //       break;
  //     }
  //     std::cout << c;
  //   }
  //   std::cout << std::endl;
  // }

  // 5. Calculate the prefix sums of inside tile areas in the compressed grid:
  // prefix_sums[x + 1][y + 1] = area of all inside tiles in rectangle (0,0) to
  // (x,y) (inclusive)
  std::vector<std::vector<uint64_t>> prefix_sums(
      compressed_width + 2, std::vector<uint64_t>(compressed_height + 2, 0u));
  const auto is_compressed_tile_inside = [&](auto x, auto y) {
    return grid[x][y] != CompressedTileType::OUTSIDE;
  };
  for (size_t x = 1; x < compressed_width + 2; ++x) {
    for (size_t y = 1; y < compressed_height + 2; ++y) {
      prefix_sums[x][y] = prefix_sums[x - 1][y] + prefix_sums[x][y - 1] -
                          prefix_sums[x - 1][y - 1] +
                          is_compressed_tile_inside(x - 1, y - 1);
    }
  }
  // 6. Finally, enumerate all possible rectangles and find the maximum area
  uint64_t max_area = 0;
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = i + 1; j < n; ++j) {
      const auto area = tiles[i].rectangle_area_to(tiles[j]);
      if (area <= max_area) {
        continue;
      }
      const auto x1 = compressed_x.compress(std::min(tiles[i].x, tiles[j].x));
      const auto y1 = compressed_y.compress(std::min(tiles[i].y, tiles[j].y));
      const auto x2 = compressed_x.compress(std::max(tiles[i].x, tiles[j].x));
      const auto y2 = compressed_y.compress(std::max(tiles[i].y, tiles[j].y));
      // the rectangle is inside iff its area equals to the number of inside
      // tiles in the compressed grid
      const auto compressed_inside_tile_count =
          prefix_sums[x2 + 1][y2 + 1] + prefix_sums[x1][y1] -
          prefix_sums[x1][y2 + 1] - prefix_sums[x2 + 1][y1];
      const auto compressed_area = (x2 - x1 + 1) * (y2 - y1 + 1);
      if (compressed_inside_tile_count == compressed_area) {
        max_area = area;
      }
    }
  }
  return max_area;
}

int main() {
  std::vector<Tile<int64_t>> tiles{
      std::istream_iterator<Tile<int64_t>>(std::cin),
      std::istream_iterator<Tile<int64_t>>()};
  std::cout << "Number of red tiles: " << tiles.size() << std::endl;
  std::cout << "Part 1: " << part_1_naive(tiles) << std::endl;
  std::cout << "Part 2: " << part_2(tiles) << std::endl;
  return 0;
}
