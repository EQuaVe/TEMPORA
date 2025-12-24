/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/transducer/state.hpp"

namespace mitl2gta {

namespace transducer {

std::string name_from_id(mitl2gta::id_t const id) {
  return "loc" + std::to_string(id);
}

state_t::state_t(mitl2gta::transducer::state_id_allocator_t &allocator)
    : _id(allocator.allocate_id()), _is_initial(false), _is_final(false),
      _is_urgent(false) {}

mitl2gta::transducer::state_t &state_t::make_initial() {
  _is_initial = true;
  return *this;
}

mitl2gta::transducer::state_t &state_t::make_final() {
  _is_final = true;
  return *this;
}

mitl2gta::transducer::state_t &state_t::make_urgent() {
  _is_urgent = true;
  return *this;
}

} // namespace transducer

} // namespace mitl2gta
