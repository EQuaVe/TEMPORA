/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>

#include "mitl2gta/gta_system/variable.hpp"
#include "mitl2gta/interval/bound.hpp"
#include "mitl2gta/utils/id_allocator.hpp"

namespace mitl2gta {

namespace memory {

using memory_id_t = mitl2gta::id_t;

enum memory_type_t {
  LOCAL_TO_SINGLE_ROUND,
  PERSISTENT,
};

struct memory_requirements_t {
  mitl2gta::gta::integer_variable_range_t range;
  mitl2gta::boundval_t initial_value;
  mitl2gta::memory::memory_type_t type;
};

class memory_handler_t : mitl2gta::id_allocator_t {
public:
  memory_handler_t() = default;
  ~memory_handler_t() = default;

  memory_handler_t(memory_handler_t const &) = delete;
  memory_handler_t &operator=(memory_handler_t const &) = delete;
  memory_handler_t(memory_handler_t &&) = default;
  memory_handler_t &operator=(memory_handler_t &&) = default;

  mitl2gta::memory::memory_id_t request_memory(
      mitl2gta::memory::memory_requirements_t const memory_requirements);

  mitl2gta::memory::memory_requirements_t const &
  memory_requirements(mitl2gta::memory::memory_id_t const id) const;

  std::size_t allocated_memory() const;

private:
  std::map<mitl2gta::memory::memory_id_t,
           mitl2gta::memory::memory_requirements_t>
      _id_to_requirements;
};

} // namespace memory
} // namespace mitl2gta
