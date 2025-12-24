/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#include "mitl2gta/clock/clock_registry.hpp"

namespace mitl2gta {
namespace clock {

mitl2gta::clock::clock_id_t
clock_registry_t::register_clock(mitl2gta::gta::gta_clock_type_t const type) {
  mitl2gta::clock::clock_id_t const new_id = allocate_id();
  _clocks.insert({new_id, type});

  return new_id;
};

mitl2gta::gta::gta_clock_type_t
clock_registry_t::type(mitl2gta::clock::clock_id_t const id) const {
  return _clocks.at(id);
}

std::size_t clock_registry_t::clocks_registered() const {
  return _clocks.size();
}

std::string clock_name(mitl2gta::clock::clock_id_t const id) {
  return "clk" + std::to_string(id);
}

std::vector<mitl2gta::gta::gta_clock_variable_t>
clock_vars_from_reg(mitl2gta::clock::clock_registry_t const &reg) {
  std::size_t num_clocks = reg.clocks_registered();

  std::vector<mitl2gta::gta::gta_clock_variable_t> vars;
  vars.reserve(num_clocks);

  for (std::size_t x = 0; x < num_clocks; x++) {
    vars.emplace_back(mitl2gta::gta::gta_clock_variable_t(
        mitl2gta::clock::clock_name(x), reg.type(x)));
  }

  return vars;
}

} // namespace clock
} // namespace mitl2gta
