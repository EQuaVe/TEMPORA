/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#include "mitl2gta/memory/memory_handler.hpp"

namespace mitl2gta {

namespace memory {

mitl2gta::memory::memory_id_t
mitl2gta::memory::memory_handler_t::request_memory(
    mitl2gta::memory::memory_requirements_t const memory_type) {
  mitl2gta::memory::memory_id_t const new_id = allocate_id();
  _id_to_requirements.insert({new_id, memory_type});

  return new_id;
}

mitl2gta::memory::memory_requirements_t const &
memory_handler_t::memory_requirements(
    mitl2gta::memory::memory_id_t const id) const {
  return _id_to_requirements.at(id);
}

std::size_t memory_handler_t::allocated_memory() const {
  return _id_to_requirements.size();
}

} // namespace memory

} // namespace mitl2gta
