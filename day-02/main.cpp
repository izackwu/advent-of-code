#include <iostream>
#include <limits>
#include <vector>

constexpr auto number_delimiter = '-', range_delimiter = ',';
// we only expect up to 10 digits, as we hardcode the dividers below
constexpr auto expected_max_number{9'999'999'999};

// If we break down an invalid (repetitive) number like "987987987", it's "987"
// repeated three times, equivalent to 987 + 987 * 1000 + 987 * 1000000 = 987 *
// 1001001.
//
// Therefore, we build the table:
//
// substr len -> repeat counts -> divider
// 1          -> 2 ... 10      -> 11, 111, ..., 11111111111
// 2          -> 2 ... 5       -> 101, 10101, ..., 101010101
// 3          -> 2 ... 3       -> 1001, 1001001
// 4          -> 2             -> 10001
// 5          -> 2             -> 100001

const std::vector<std::vector<int>> dividers = {
    {11, 111, 1111, 11111, 111111, 1111111, 11111111, 111111111, 1111111111},
    {101, 10101, 1010101, 101010101},
    {1001, 1001001},
    {10001},
    {100001}};

bool is_repetitive(long long num, int repeat_count_max) {
  for (auto i = 0, min = 1; i < static_cast<int>(dividers.size());
       ++i, min *= 10) {
    for (auto j = 0;
         j < static_cast<int>(dividers[i].size()) && j + 2 <= repeat_count_max;
         ++j) {
      const auto divider = dividers[i][j];
      // check if num = x * divider, where x is a substring of the exact length
      if (num % divider == 0 && (num / divider) >= min &&
          (num / divider) < min * 10) {
        return true;
      }
    }
  }
  return false;
}

long long solve(const std::vector<std::pair<long long, long long>> &ranges,
                int repeat_count_max) {
  long long result = 0;
  for (const auto [from, to] : ranges) {
    for (auto num = from; num <= to; ++num) {
      if (is_repetitive(num, repeat_count_max)) {
        result += num;
      }
    }
  }
  return result;
}

int main() {
  std::vector<std::pair<long long, long long>> ranges;
  // input format: from-to,from2-to2
  char delimiter;
  long long max_number = 0;
  for (long long from, to; std::cin >> from >> delimiter >> to;) {
    if (delimiter != number_delimiter) {
      std::cerr << "Unexpected number delimiter: " << delimiter << std::endl;
      return 1;
    }
    ranges.emplace_back(from, to);
    max_number = std::max(max_number, to);
    // consume the trailing range delimiter
    if (std::cin >> delimiter && delimiter != range_delimiter) {
      std::cerr << "Unexpected range delimiter: " << delimiter << std::endl;
      return 1;
    }
  }
  if (max_number > expected_max_number) {
    std::cerr << "Can't handle this input. The max number in it is too large. "
                 "max_number="
              << max_number << ", which can't be larger than "
              << expected_max_number << std::endl;
    return 1;
  }
  std::cout << "Part 1: " << solve(ranges, 2) << std::endl;
  std::cout << "Part 2: " << solve(ranges, std::numeric_limits<int>::max())
            << std::endl;
  return 0;
}
