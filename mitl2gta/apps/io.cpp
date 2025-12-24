/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */
#include "io.hpp"
#include <fstream>
#include <iostream>

io_handler_t::io_handler_t(
    std::optional<std::filesystem::path> const formula_input,
    std::optional<std::filesystem::path> const gta_output)
    : _formula_input(formula_input), _gta_output(gta_output) {}

std::string io_handler_t::read_formula() const {
  std::string formula;
  if (_formula_input.has_value()) {
    std::ifstream fin(_formula_input.value());
    if (!fin.is_open()) {
      throw std::runtime_error("Unable to open file: " +
                               _formula_input.value().string());
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();
    formula = buffer.str();
  }

  else {
    std::cout
        << "No input file passed for formula. Defaulting to standard input"
        << std::endl;
    std::getline(std::cin, formula);
  }

  return formula;
}

void io_handler_t::write_system(std::string const &system) const {
  if (_gta_output.has_value()) {
    std::ofstream os(_gta_output.value());
    if (!os.is_open()) {
      throw std::runtime_error("Unable to open file: " +
                               _gta_output.value().string());
    }

    os << system << std::endl;
  }

  else {
    std::cout << system << std::endl;
  }
}
