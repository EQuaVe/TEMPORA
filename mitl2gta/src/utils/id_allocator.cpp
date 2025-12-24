/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/utils/id_allocator.hpp"

namespace mitl2gta {

id_allocator_t::id_allocator_t() : _next_id(0) {}

mitl2gta::id_t id_allocator_t::allocate_id() {
  mitl2gta::id_t const next_id = _next_id;
  _next_id++;
  return next_id;
}

} // namespace mitl2gta
