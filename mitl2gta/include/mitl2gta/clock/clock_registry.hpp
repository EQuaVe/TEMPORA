/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>
#include <vector>

#include "mitl2gta/gta_system/variable.hpp"
#include "mitl2gta/utils/id_allocator.hpp"

namespace mitl2gta {

namespace clock {

using clock_id_t = std::size_t;

class clock_registry_t : mitl2gta::id_allocator_t {

public:
  clock_registry_t() = default;
  ~clock_registry_t() = default;

  clock_registry_t(clock_registry_t const &) = delete;
  clock_registry_t(clock_registry_t &&) = default;
  clock_registry_t &operator=(clock_registry_t const &) = delete;
  clock_registry_t &operator=(clock_registry_t &&) = default;

  mitl2gta::clock::clock_id_t
  register_clock(mitl2gta::gta::gta_clock_type_t const type);

  mitl2gta::gta::gta_clock_type_t
  type(mitl2gta::clock::clock_id_t const id) const;

  std::size_t clocks_registered() const;

private:
  std::map<mitl2gta::clock::clock_id_t, enum mitl2gta::gta::gta_clock_type_t>
      _clocks;
};

std::string clock_name(mitl2gta::clock::clock_id_t const id);

std::vector<mitl2gta::gta::gta_clock_variable_t>
clock_vars_from_reg(mitl2gta::clock::clock_registry_t const &reg);

} // namespace clock

} // namespace mitl2gta
