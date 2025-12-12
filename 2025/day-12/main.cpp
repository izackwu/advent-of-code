#include <iostream>
#include <istream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <vector>

constexpr uint shape_size = 3;
constexpr uint num_shapes = 6;

struct Shape {
  uint num_pixels;
  // this problem is just prank - we don't actually need to store the shape, but
  // just the number of pixels
  friend std::istream &operator>>(std::istream &is, Shape &shape) {
    uint num_pixels{0};
    std::string line;
    uint line_count = 0;
    while (line_count < shape_size) {
      std::getline(is, line);
      if (line.empty()) {
        continue; // skip empty lines
      }
      if (line.size() != shape_size) {
        throw std::runtime_error("Expected line of length " +
                                 std::to_string(shape_size) + " but got " +
                                 std::to_string(line.size()));
      }
      for (const auto c : line) {
        if (c == '#') {
          ++num_pixels;
        }
      }
      ++line_count;
    }
    shape.num_pixels = num_pixels;
    return is;
  }
};

auto part_1(const std::vector<Shape> &shapes, uint width, uint height,
            const std::vector<uint> &shape_counts) {
  // If the region is large enough to fit all the shapes into their 3x3 blocks,
  // it definitely can fit
  const auto num_blocks = (width / shape_size) * (height / shape_size);
  if (num_blocks >= std::reduce(shape_counts.begin(), shape_counts.end())) {
    return true;
  }
  // If the region is too small to fit all the pixels (ignoring shape
  // arrangement), it definitely can't fit
  const auto total_pixels_needed = std::inner_product(
      shapes.begin(), shapes.end(), shape_counts.begin(), 0u, std::plus<>(),
      [](const auto &shape, const auto count) {
        return shape.num_pixels * count;
      });
  if (total_pixels_needed > width * height) {
    return false;
  }
  // If neither of the above conditions hold, it may or may not fit, and it's a
  // NP-complete problem
  throw std::runtime_error(
      "Cannot determine whether the shapes can fit in the region.");
}

int main() {
  std::vector<Shape> shapes;
  for (uint i = 0; i < num_shapes; ++i) {
    uint index;
    char colon;
    std::cin >> index >> colon;
    if (colon != ':') {
      throw std::runtime_error("Expected ':' but got " + std::string(1, colon));
    }
    if (index != i) {
      throw std::runtime_error("Expected shape index " + std::to_string(i) +
                               " but got " + std::to_string(index));
    }
    Shape shape;
    std::cin >> shape;
    shapes.push_back(shape);
  }
  // print shapes info
  for (uint i = 0; i < num_shapes; ++i) {
    std::cout << "Shape " << i << ": " << shapes[i].num_pixels << " pixels"
              << std::endl;
  }
  uint64_t can_fit_count = 0;
  for (std::string line; std::getline(std::cin, line);) {
    if (line.empty()) {
      continue; // skip empty lines
    }
    std::istringstream iss(line);
    char separator;
    uint width, height;
    iss >> width >> separator >> height;
    if (separator != 'x') {
      throw std::runtime_error("Expected 'x' but got " +
                               std::string(1, separator));
    }
    iss >> separator;
    if (separator != ':') {
      throw std::runtime_error("Expected ':' but got " +
                               std::string(1, separator));
    }
    std::vector<uint> shape_counts{std::istream_iterator<uint>(iss),
                                   std::istream_iterator<uint>()};
    if (shape_counts.size() != num_shapes) {
      throw std::runtime_error("Expected " + std::to_string(num_shapes) +
                               " shape counts but got " +
                               std::to_string(shape_counts.size()));
    }
    can_fit_count += part_1(shapes, width, height, shape_counts);
  }
  std::cout << "Part 1: " << can_fit_count << std::endl;
  return 0;
}
