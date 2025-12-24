/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/transducer/edge.hpp"

namespace mitl2gta {
namespace transducer {

edge_t::edge_t(mitl2gta::transducer::state_id_t from,
               mitl2gta::transducer::state_id_t to,
               mitl2gta::transducer::transition_on_t on,
               std::vector<mitl2gta::transducer::transition_guard_t> guards,
               std::vector<mitl2gta::transducer::transition_action_t> actions,
               std::vector<mitl2gta::transducer::gta_program_t> gta_program)
    : _from(from), _to(to), _on(std::move(on)), _guards(std::move(guards)),
      _actions(std::move(actions)), _gta_program(std::move(gta_program)){}

;

} // namespace transducer
} // namespace mitl2gta
