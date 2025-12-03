#include <iostream>
#include <ranges>
#include <string>
#include <vector>

int part_1(const std::vector<std::string> &banks) {
  int total_joltage = 0;
  for (const auto &bank : banks) {
    if (banks.size() < 2) {
      std::cerr
          << "Invalid input: the battery bank has fewer than 2 batteries: "
          << bank << std::endl;
      return -1;
    }
    char max = 0, sub_max = bank.back();
    for (auto b : bank | std::views::reverse | std::views::drop(1)) {
      if (b >= max) {
        sub_max = std::max(sub_max, max);
        max = b;
      }
    }
    const auto joltage = ((max - '0') * 10 + (sub_max - '0'));
    total_joltage += joltage;
  }
  return total_joltage;
}

// Naive solution, just do multiple `max_element`s per battery bank
// Time: O(N * digits) per bank
long long part_2_naive(const std::vector<std::string> &banks,
                       const int digits) {
  long long total_joltage = 0;
  for (const auto &bank : banks) {
    if (static_cast<int>(bank.size()) < digits) {
      std::cerr
          << "Invalid input! The battery bank doesn't have enough batteries: "
          << bank << std::endl;
      return -1;
    }
    long long joltage = 0;
    auto lo = bank.cbegin();
    for (int i = 0; i < digits; ++i) {
      const auto hi = bank.cend() - digits + i + 1;
      const auto iter = std::max_element(lo, hi);
      joltage = joltage * 10 + (*iter - '0');
      lo = iter + 1;
    }
    // std::cout << bank << " : " << joltage << std::endl;
    total_joltage += joltage;
  }
  return total_joltage;
}

// Optimized solution using a monotonic stack
// Time: O(N) per bank
long long part_2_optimized(const std::vector<std::string> &banks,
                           const int digits) {
  long long total_joltage = 0;
  for (const auto &bank : banks) {
    if (static_cast<int>(bank.size()) < digits) {
      std::cerr
          << "Invalid input! The battery bank doesn't have enough batteries: "
          << bank << std::endl;
      return -1;
    }

    std::deque<char> mono_stack;
    int removals = bank.size() - digits;
    for (const auto b : bank) {
      while (!mono_stack.empty() && removals > 0 && mono_stack.back() < b) {
        mono_stack.pop_back();
        --removals;
      }
      mono_stack.push_back(b);
    }
    // only keep the top `digits` elements
    long long joltage = 0;
    for (int i = 0; i < digits; ++i) {
      joltage = joltage * 10 + (mono_stack[i] - '0');
    }
    total_joltage += joltage;
  }
  return total_joltage;
}

int main() {
  std::vector<std::string> banks;
  for (std::string bank; std::cin >> bank;) {
    banks.emplace_back(bank);
  }
  std::cout << "Number of battery banks: " << banks.size() << std::endl;
  std::cout << "Part 1: " << part_1(banks) << std::endl;
  std::cout << "Part 2: " << part_2_naive(banks, 12) << std::endl;
  std::cout << "Part 2: " << part_2_optimized(banks, 12) << std::endl;
  return 0;
}
