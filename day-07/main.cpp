#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

auto part_1(const std::vector<std::vector<bool>> &manifold, auto start_pos) {
  const auto cols = manifold[0].size();
  int split_count = 0;
  std::vector<bool> beams(cols, false);
  beams[start_pos] = true;
  for (const auto &row : manifold) {
    decltype(beams) new_beams(cols, false);
    for (size_t col = 0; col < cols; ++col) {
      if (!beams[col]) {
        continue;
      }
      if (!row[col]) { // vertical path continues
        new_beams[col] = true;
      } else { // split
        if (col > 0) {
          new_beams[col - 1] = true;
        }
        if (col + 1 < cols) {
          new_beams[col + 1] = true;
        }
        ++split_count;
      }
    }
    std::swap(beams, new_beams);
  }
  return split_count;
}

auto part_2(const std::vector<std::vector<bool>> &manifold, auto start_pos) {
  const auto cols = manifold[0].size();
  std::vector<uint64_t> timelines(cols, 0);
  timelines[start_pos] = 1;
  for (const auto &row : manifold) {
    decltype(timelines) new_timelines(cols, 0);
    for (size_t col = 0; col < cols; ++col) {
      if (timelines[col] == 0) {
        continue;
      }
      if (!row[col]) { // vertical path continues
        new_timelines[col] += timelines[col];
      } else { // split
        if (col > 0) {
          new_timelines[col - 1] += timelines[col];
        }
        if (col + 1 < cols) {
          new_timelines[col + 1] += timelines[col];
        }
      }
    }
    std::swap(timelines, new_timelines);
  }
  return std::reduce(timelines.cbegin(), timelines.cend());
}

int main() {
  std::string line;
  std::cin >> line;
  // find the starting point
  const auto start_pos = line.find('S');
  if (start_pos == std::string::npos) {
    throw std::runtime_error("No starting point 'S' found in input");
  }
  std::vector<std::vector<bool>> manifold;
  while (std::cin >> line) {
    manifold.emplace_back(
        line | std::views::transform([](char c) { return c == '^'; }) |
        std::ranges::to<std::vector<bool>>());
  }
  std::cout << "Start position: " << start_pos << std::endl;
  // sanity check: manifold should be a rectangle
  const auto row_size = manifold[0].size();
  for (const auto &row : manifold) {
    if (row.size() != row_size) {
      throw std::runtime_error("Manifold rows are not the same size");
    }
  }
  // also, there shouldn't be neighboring splits in the same row
  for (const auto &row : manifold) {
    for (size_t col = 1; col < row.size(); ++col) {
      if (row[col] && row[col - 1]) {
        throw std::runtime_error(
            "Manifold has neighboring splits in the same row at column " +
            std::to_string(col));
      }
    }
  }
  std::cout << "Manifold size: " << manifold.size() << " x "
            << manifold[0].size() << std::endl;
  std::cout << "Part 1: " << part_1(manifold, start_pos) << std::endl;

  std::cout << "Part 1: " << part_2(manifold, start_pos) << std::endl;
  return 0;
}
