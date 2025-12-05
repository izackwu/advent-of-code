#include <algorithm>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

template <typename T> struct Interval {
  T start;
  T end;

  bool operator<(const Interval &other) const {
    return start < other.start || (start == other.start && end < other.end);
  }

  // input format : start-end
  friend std::istream &operator>>(std::istream &is, Interval &interval) {
    char dash;
    is >> interval.start >> dash >> interval.end;
    // make sure the separator is dash and the interval is valid
    if (dash != '-' || interval.start > interval.end) {
      is.setstate(std::ios::failbit);
    }
    return is;
  }
};

template <typename T>
int part_1(const std::vector<Interval<T>> &intervals,
           const std::vector<T> &values) {
  int total = 0;
  for (const auto &value : values) {
    for (const auto &interval : intervals) {
      if (value >= interval.start && value <= interval.end) {
        ++total;
        break;
      }
    }
  }
  return total;
}

template <typename T> T part_2(std::vector<Interval<T>> intervals) {
  if (intervals.empty()) {
    return 0;
  }
  std::sort(intervals.begin(), intervals.end());
  T total = 0;
  T lo = intervals.front().start, hi = intervals.front().end;
  for (const auto &interval : intervals | std::views::drop(1)) {
    if (interval.start > hi) {
      total += hi - lo + 1;
      lo = interval.start;
      hi = interval.end;
    } else {
      hi = std::max(hi, interval.end);
    }
  }
  total += hi - lo + 1;
  return total;
}

int main() {
  using value_t = uint64_t;
  std::vector<Interval<value_t>> intervals;
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    std::istringstream iss{line};
    Interval<value_t> interval;
    if (!(iss >> interval)) {
      std::cerr << "Invalid interval input: " << line << std::endl;
      return 1;
    }
    intervals.emplace_back(interval);
  }
  std::vector<value_t> values;
  for (value_t v; std::cin >> v;) {
    values.emplace_back(v);
  }
  std::cout << "Total intervals: " << intervals.size() << std::endl;
  std::cout << "Total values: " << values.size() << std::endl;
  std::cout << "Part 1: " << part_1(intervals, values) << std::endl;
  std::cout << "Part 2: " << part_2(intervals) << std::endl;
  return 0;
}
