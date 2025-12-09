#include <iostream>
#include <vector>

constexpr int SIZE = 100;

int part_1(const std::vector<std::pair<char, int>> &moves) {
  int zero_count = 0, pos = 50;
  for (const auto &[dir, steps] : moves) {
    if (dir == 'L') {
      pos = (pos - steps + SIZE) % SIZE;
    } else if (dir == 'R') {
      pos = (pos + steps) % SIZE;
    }
    if (pos == 0) {
      ++zero_count;
    }
  }
  return zero_count;
}

int part_2_simple(const std::vector<std::pair<char, int>> &moves) {
  int zero_count = 0, pos = 50;
  for (const auto &[dir, steps] : moves) {
    for (int i = 0; i < steps; ++i) {
      if (dir == 'L') {
        pos = (pos - 1 + SIZE) % SIZE;
      } else if (dir == 'R') {
        pos = (pos + 1) % SIZE;
      }
      if (pos == 0) {
        ++zero_count;
      }
    }
  }
  return zero_count;
}

int part_2_optimized(const std::vector<std::pair<char, int>> &moves) {
  int zero_count = 0, pos = 50;
  for (const auto &[dir, steps] : moves) {
    const auto full_cycles = steps / SIZE, remaining_steps = steps % SIZE;
    zero_count += full_cycles;
    // now we will only go through / arrive at 0 at most once
    if (remaining_steps == 0) {
      continue;
    }
    if (dir == 'L') {
      // if pos is already 0, then we can't hit it again in the remaining steps
      zero_count += (pos > 0 && pos <= remaining_steps);
      pos = (pos - remaining_steps + SIZE) % SIZE;
    } else if (dir == 'R') {
      zero_count += (pos + remaining_steps >= SIZE);
      pos = (pos + remaining_steps) % SIZE;
    }
  }
  return zero_count;
}

int main() {
  std::vector<std::pair<char, int>> moves;
  char dir;
  int steps;
  while (std::cin >> dir >> steps) {
    moves.emplace_back(dir, steps);
  }
  std::cout << "Number of moves: " << moves.size() << std::endl;
  std::cout << "Part 1: " << part_1(moves) << std::endl;
  const auto part_2_simple_ = part_2_simple(moves),
             part_2_optimized_ = part_2_optimized(moves);
  if (part_2_simple_ != part_2_optimized_) {
    std::cerr << "Part 2 implementations do not match: simple="
              << part_2_simple_ << ", optimized=" << part_2_optimized_
              << std::endl;
    return 1;
  }
  std::cout << "Part 2: " << part_2_simple_ << std::endl;
  return 0;
}
