/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <array>
#include <stdexcept>

#include "mitl2gta/gta_system/variable.hpp"
#include "mitl2gta/utils/string_construction.hpp"

namespace mitl2gta {
namespace gta {

// variable_t
variable_t::variable_t(std::string name) : _name(std::move(name)) {
  if (_name.empty()) {
    throw std::invalid_argument("Empty name invalid for variable");
  }
}
// integer_variable_range_t

integer_variable_range_t::integer_variable_range_t(
    mitl2gta::boundval_t const lower_bound,
    mitl2gta::boundval_t const upper_bound)
    : mitl2gta::nonempty_interval_t(
          mitl2gta::finite_bound(lower_bound,
                                 mitl2gta::bound_type_t::CLOSED_BOUND),
          mitl2gta::finite_bound(upper_bound,
                                 mitl2gta::bound_type_t::CLOSED_BOUND)) {}

// array_integer_variables_t
array_integer_variables_t::array_integer_variables_t(
    std::string name, std::size_t size,
    mitl2gta::gta::integer_variable_range_t range,
    mitl2gta::boundval_t initial_value)
    : mitl2gta::gta::variable_t(std::move(name)), _size(size),
      _range(std::move(range)), _initial_value(initial_value) {
  mitl2gta::boundval_t const lower_bound = range.lower_bound_value();
  mitl2gta::boundval_t const upper_bound = range.upper_bound_value();

  if (!(lower_bound <= initial_value && initial_value <= upper_bound)) {
    throw std::invalid_argument("Invalid initial value for variable: " + _name);
  }
}

std::string varname(mitl2gta::gta::array_integer_variables_t const &arr,
                    std::size_t index) {
  if (index > arr.size()) {
    throw std::invalid_argument("Invalid index for array");
  }

  if (arr.size() == 1) {
    return arr.name();
  } else {
    return arr.name() + "[" + std::to_string(index) + "]";
  }
}

// gta_clock_variable_t
gta_clock_variable_t::gta_clock_variable_t(std::string const name,
                                           enum gta_clock_type_t const type)
    : mitl2gta::gta::variable_t(std::move(name)), _type(type) {}

/*
 * Format accepted by TChecker:
 * int:size:lower_range:upper_range:initial_value:name
 */
std::string declaration(mitl2gta::gta::array_integer_variables_t const &v) {
  std::array<std::string, 6> const substrings{
      "int",
      std::to_string(v.size()),
      std::to_string(v.range().lower_bound_value()),
      std::to_string(v.range().upper_bound_value()),
      std::to_string(v.initial_value()),
      v.name()};

  return mitl2gta::join(substrings.cbegin(), substrings.cend(), ":");
}

/*
 * Format accepted by TChecker:
 * clock:1:name{type: <clk_type>}
 */
std::string declaration(mitl2gta::gta::gta_clock_variable_t const &v) {

  std::array<std::string, 3> const substrings{"clock", "1", v.name()};

  std::string type_declaration;
  switch (v.type()) {
  case mitl2gta::gta::gta_clock_type_t::PROPHECY:
    type_declaration = "prophecy";
    break;

  case mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO:
    type_declaration = "history_zero";
    break;

  case mitl2gta::gta::gta_clock_type_t::HISTORY_INF:
    type_declaration = "history_inf";
    break;

  default:
    throw std::runtime_error("Incomplete switch statement");
  }

  mitl2gta::gta::attributes_t const attributes{{"type", type_declaration}};

  return mitl2gta::join(substrings.cbegin(), substrings.cend(), ":") +
         mitl2gta::gta::attributes_to_string(attributes);
}

} // namespace gta
} // namespace mitl2gta
