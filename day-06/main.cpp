#include <cctype>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <vector>

auto part_1(const auto &operand_lines, const auto &operator_line) {
  std::vector<bool> multiply_operators;
  {
    std::istringstream iss(operator_line);
    for (std::string token; iss >> token;) {
      if (token == "+") {
        multiply_operators.push_back(false);
      } else if (token == "*") {
        multiply_operators.push_back(true);
      } else {
        throw std::runtime_error("Invalid operator: " + token);
      }
    }
  }
  const auto num_operations = multiply_operators.size();
  // parse operands
  std::vector<std::vector<uint64_t>> operands;
  for (const auto &line : operand_lines) {
    std::istringstream iss(line);
    std::vector<uint64_t> operand_row{std::istream_iterator<uint64_t>(iss),
                                      std::istream_iterator<uint64_t>()};
    if (operand_row.size() != num_operations) {
      throw std::runtime_error(
          "Operand row size does not match number of operators");
    }
    operands.push_back(operand_row);
  }
  // compute result
  uint64_t result = 0;
  for (decltype(multiply_operators.size()) i = 0; i < num_operations; ++i) {
    uint64_t op_result = operands[0][i];
    for (decltype(operands.size()) j = 1; j < operands.size(); ++j) {
      if (multiply_operators[i]) {
        op_result *= operands[j][i];
      } else {
        op_result += operands[j][i];
      }
    }
    result += op_result;
  }
  return result;
}

// parse each operator into multiply/add and its column range
struct OperatorInfo {
  bool is_multiply;
  size_t start_col; // inclusive
  size_t end_col;   // inclusive

  // output operator for debugging
  friend std::ostream &operator<<(std::ostream &os,
                                  const OperatorInfo &op_info) {
    os << (op_info.is_multiply ? "*" : "+") << " [" << op_info.start_col << ", "
       << op_info.end_col << "]";
    return os;
  }
};

auto part_2(const auto &operand_lines, const auto &operator_line) {
  // asset all lines are of the same length
  const auto line_length = operator_line.size();
  for (const auto &line : operand_lines) {
    if (line.size() != line_length) {
      throw std::runtime_error("Operand line length does not match operator "
                               "line length");
    }
  }
  std::vector<OperatorInfo> operators;
  for (auto last_op = operator_line.cbegin();
       last_op != operator_line.cend();) {
    const auto next_op =
        std::find_if(last_op + 1, operator_line.cend(),
                     [](char c) { return c == '+' || c == '*'; });
    const auto is_last = (next_op == operator_line.cend());

    // add the last operator
    operators.emplace_back(OperatorInfo{
        .is_multiply = (*last_op == '*'),
        .start_col =
            static_cast<size_t>(std::distance(operator_line.cbegin(), last_op)),
        .end_col = is_last ? static_cast<size_t>(operator_line.size() - 1)
                           : static_cast<size_t>(std::distance(
                                 operator_line.cbegin(), next_op)) -
                                 2});
    last_op = next_op;
  }
  // for debugging, print parsed operators
  //   std::cout << "\nParsed " << operators.size() << " operators" <<
  //   std::endl; for (const auto &op_info : operators) {
  //     std::cout << op_info << std::endl;
  //   }
  // compute result
  uint64_t result = 0;
  for (const auto &op_info : operators) {
    // std::cout << "Processing operator: " << op_info << std::endl;
    uint64_t op_result = op_info.is_multiply;
    for (int col = op_info.end_col; col >= static_cast<int>(op_info.start_col);
         --col) {
      uint64_t operand = 0;
      for (const auto &line : operand_lines) {
        const auto c = line[col];
        if (std::isdigit(c)) {
          operand = operand * 10 + (c - '0');
        }
      }
      //   std::cout << operand << std::endl;
      // sanity check, operand shouldn't be zero
      if (operand == 0) {
        throw std::runtime_error("Parsed operand is zero at column " +
                                 std::to_string(col));
      }
      if (op_info.is_multiply) {
        op_result *= operand;
      } else {
        op_result += operand;
      }
    }
    result += op_result;
  }

  return result;
}

int main() {
  std::vector<std::string> input_lines;
  for (std::string line; std::getline(std::cin, line);) {
    input_lines.push_back(line);
  }
  // at least 2 lines
  if (input_lines.size() < 2) {
    std::cerr << "Insufficient input data. There should be at least one line "
                 "of operands and one line of operators."
              << std::endl;
    return 1;
  }
  const auto &operator_line = input_lines.back();
  const auto &operand_lines =
      input_lines | std::views::take(input_lines.size() - 1);
  std::cout << "Part 1: " << part_1(operand_lines, operator_line) << std::endl;
  std::cout << "Part 2: " << part_2(operand_lines, operator_line) << std::endl;
  return 0;
}
