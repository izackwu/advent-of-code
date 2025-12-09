#include <iostream>
#include <numeric>
#include <vector>

auto part_1(const std::vector<uint64_t> &modules) {
  return std::transform_reduce(modules.cbegin(), modules.cend(), 0ul,
                               std::plus<uint64_t>{},
                               [](auto mass) { return (mass / 3) - 2; });
}

auto part_2(const std::vector<uint64_t> &modules) {
  return std::transform_reduce(modules.cbegin(), modules.cend(), 0ul,
                               std::plus<uint64_t>{}, [](auto mass) {
                                 uint64_t total_fuel = 0;
                                 while (mass >= 9) {
                                   mass = (mass / 3) - 2;
                                   total_fuel += mass;
                                 }
                                 return total_fuel;
                               });
}

int main() {
  std::vector<uint64_t> modules;
  for (uint64_t mass; std::cin >> mass;) {
    modules.push_back(mass);
  }
  std::cout << "Part 1: " << part_1(modules) << std::endl;
  std::cout << "Part 2: " << part_2(modules) << std::endl;
  return 0;
}
