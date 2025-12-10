#include "Highs.h"
#include <bitset>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

// there can be only up to 10 lights
constexpr size_t max_light_num = 10;

using Lights = std::bitset<max_light_num>;
using Button = std::bitset<max_light_num>;
using Joltages = std::array<uint, max_light_num>;

// BFS
// Time: O(N), where N is the number of possible states (2 ** light_num)
// Space: O(N)
auto part_1(const Lights target_lights, const std::vector<Button> &buttons) {
  if (target_lights == 0) {
    return 0u;
  }
  std::bitset<(1 << max_light_num)> visited{0};
  visited.set(0);
  std::vector<Lights> current{
      0,
  };
  uint operations = 0;
  while (!current.empty()) {
    decltype(current) next;
    ++operations;
    for (const auto state : current) {
      for (const auto button : buttons) {
        const auto next_state = state ^ button;
        if (next_state == target_lights) {
          return operations;
        }
        if (!visited.test(next_state.to_ulong())) {
          visited.set(next_state.to_ulong());
          next.push_back(next_state);
        }
      }
    }
    std::swap(current, next);
  }
  throw std::runtime_error("The indictor light state can't be achieved.");
}

auto part_2(const Joltages &target_joltages,
            const std::vector<Button> &buttons) {

  // model this as an ILP problem:
  // Objective:
  //   Min x1 + x2 + ... + xn, where xi is the number of the i-th button press
  // Constraints:
  //   Each machine's joltage is exactly satisfied

  Highs highs;
  highs.setOptionValue("output_flag", false);
  // 1. Define Variables (Columns)
  // We loop through the known number of variables and add them to the solver.
  for (size_t i = 0; i < buttons.size(); ++i) {
    // We add them with 0 constraints initially (num_non_zeros = 0).
    highs.addCol(1.0, 0.0, kHighsInf, 0, nullptr, nullptr);

    // Mark as Integer immediately
    highs.changeColIntegrality(i, HighsVarType::kInteger);
  }

  // 2. Define Constraints (Rows)
  // We add equations one by one.
  for (size_t j = 0; j < max_light_num; ++j) {
    // Unzip the C++ pair into C-style arrays for the API
    std::vector<int> indices;
    std::vector<double> values;

    for (size_t i = 0; i < buttons.size(); ++i) {
      if (buttons[i][j]) {
        indices.push_back(i);
        values.push_back(1);
      }
    }
    if (indices.empty()) {
      continue;
    }

    // For an EQUALITY (Ax = b), Lower == Upper == b
    highs.addRow(target_joltages[j], target_joltages[j], (int)indices.size(),
                 indices.data(), values.data());
  }

  //   highs.writeModel("debug_model.lp");
  highs.run();

  if (highs.getModelStatus() != HighsModelStatus::kOptimal) {
    throw std::runtime_error("Couldn't find an optimal solution. Why?");
  }
  return highs.getInfo().objective_function_value;
}

int main() {
  uint light_operations{0}, joltage_operations{0};
  for (std::string line; std::getline(std::cin, line);) {
    std::istringstream iss{line};
    Lights lights{0};
    std::vector<Button> buttons;
    char c;
    iss >> c;
    if (c != '[') {
      throw std::runtime_error("Expect '[' but got " + std::string(1, c));
    }
    // read the target indictor lights
    for (size_t i = 0; i < max_light_num + 1; ++i) {
      iss >> c;
      if (c == '.') {
        lights[i] = false;
      } else if (c == '#') {
        lights[i] = true;
      } else if (c == ']') {
        break;
      } else {
        throw std::runtime_error(
            "Got some unexpected character while reading the lights: " +
            std::string(1, c));
      }
    }
    // read the buttons
    while (iss >> c && c == '(') {
      Button button{0};
      uint d;
      while (iss >> d >> c) {
        button[d] = true;
        if (c == ',') {
          // read on
        } else if (c == ')') {
          break;
        } else {
          throw std::runtime_error(
              "Got some unexpected character while reading the buttons: " +
              std::string(1, c));
        }
      }
      buttons.push_back(button);
    }
    // read the joltage requirements
    if (c != '{') {
      throw std::runtime_error("Expect '{' but got " + std::string(1, c));
    }
    Joltages joltages;
    uint d;
    for (size_t i = 0; i < max_light_num; ++i) {
      iss >> d >> c;
      joltages[i] = d;
      if (c == ',') {
        // read on
      } else if (c == '}') {
        break;
      } else {
        throw std::runtime_error(
            "Got some unexpected character while reading the joltages: " +
            std::string(1, c));
      }
    }
    light_operations += part_1(lights, buttons);
    joltage_operations += part_2(joltages, buttons);
  }
  std::cout << "Part 1: " << light_operations << std::endl;
  std::cout << "Part 1: " << joltage_operations << std::endl;
  return 0;
}
