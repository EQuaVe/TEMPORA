/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <string>

#include "mitl2gta/interval/bound.hpp"
#include "mitl2gta/interval/interval.hpp"

namespace mitl2gta {

namespace gta {

class variable_t {
public:
  explicit variable_t(std::string name);

  variable_t(variable_t const &) = default;
  variable_t(variable_t &&) = default;
  variable_t &operator=(variable_t const &) = default;
  variable_t &operator=(variable_t &&) = default;

  inline std::string name() const { return _name; }

protected:
  std::string _name;
};

class integer_variable_range_t : mitl2gta::nonempty_interval_t {
public:
  integer_variable_range_t(mitl2gta::boundval_t const lower_bound,
                           mitl2gta::boundval_t const upper_bound);

  integer_variable_range_t(integer_variable_range_t const &) = default;
  integer_variable_range_t(integer_variable_range_t &&) = default;
  integer_variable_range_t &
  operator=(integer_variable_range_t const &) = default;
  integer_variable_range_t &operator=(integer_variable_range_t &&) = default;

  inline mitl2gta::boundval_t lower_bound_value() const {
    return lower_bound().val;
  }

  inline mitl2gta::boundval_t upper_bound_value() const {
    return upper_bound().val;
  }
};

class array_integer_variables_t : public mitl2gta::gta::variable_t {
public:
  ~array_integer_variables_t() = default;
  array_integer_variables_t(std::string name, std::size_t size,
                            mitl2gta::gta::integer_variable_range_t range,
                            mitl2gta::boundval_t initial_value);

  array_integer_variables_t(array_integer_variables_t const &) = default;
  array_integer_variables_t(array_integer_variables_t &&) = default;
  array_integer_variables_t &
  operator=(array_integer_variables_t const &) = default;
  array_integer_variables_t &operator=(array_integer_variables_t &&) = default;

  inline std::size_t size() const { return _size; }
  mitl2gta::gta::integer_variable_range_t range() const { return _range; }
  inline mitl2gta::boundval_t initial_value() const { return _initial_value; }

private:
  std::size_t _size;
  mitl2gta::gta::integer_variable_range_t _range;
  mitl2gta::boundval_t _initial_value;
};

std::string varname(mitl2gta::gta::array_integer_variables_t const &arr,
                    std::size_t index);

enum gta_clock_type_t {
  HISTORY_ZERO,
  HISTORY_INF,
  PROPHECY,
};

class gta_clock_variable_t : public mitl2gta::gta::variable_t {
public:
  gta_clock_variable_t(std::string const name,
                       enum gta_clock_type_t const type);

  gta_clock_variable_t(gta_clock_variable_t const &) = default;
  gta_clock_variable_t(gta_clock_variable_t &&) = default;
  gta_clock_variable_t &operator=(gta_clock_variable_t const &) = default;
  gta_clock_variable_t &operator=(gta_clock_variable_t &&) = default;

  ~gta_clock_variable_t() = default;

  inline mitl2gta::gta::gta_clock_type_t type() const { return _type; }

private:
  mitl2gta::gta::gta_clock_type_t _type;
};

std::string declaration(mitl2gta::gta::array_integer_variables_t const &v);
std::string declaration(mitl2gta::gta::gta_clock_variable_t const &v);

} // namespace gta

} // namespace mitl2gta
