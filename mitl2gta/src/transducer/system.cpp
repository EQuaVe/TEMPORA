/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/transducer/system.hpp"

namespace mitl2gta {

namespace transducer {

system_t::system_t(mitl2gta::transducer::system_for_t const system_for)
    : _system_for(system_for) {}

void system_t::add_transducer(
    mitl2gta::transducer::transducer_t const transducer) {
  _transducers.push_back(transducer);
}

} // namespace transducer

} // namespace mitl2gta
