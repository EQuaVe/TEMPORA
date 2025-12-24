/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <vector>

#include "mitl2gta/transducer/edge_instructions.hpp"
#include "mitl2gta/transducer/state.hpp"

namespace mitl2gta {
namespace transducer {

class edge_t {

public:
  edge_t(mitl2gta::transducer::state_id_t from,
         mitl2gta::transducer::state_id_t to,
         mitl2gta::transducer::transition_on_t on,
         std::vector<mitl2gta::transducer::transition_guard_t> guards,
         std::vector<mitl2gta::transducer::transition_action_t> actions,
         std::vector<mitl2gta::transducer::gta_program_t> gta_program);

  ~edge_t() = default;
  edge_t(edge_t const &) = default;
  edge_t(edge_t &&) = default;
  edge_t &operator=(edge_t const &) = default;
  edge_t &operator=(edge_t &&) = default;

  inline mitl2gta::transducer::state_id_t from() const { return _from; }

  inline mitl2gta::transducer::state_id_t to() const { return _to; }

  inline std::vector<mitl2gta::transducer::transition_guard_t> &guards() {
    return _guards;
  }

  inline std::vector<mitl2gta::transducer::transition_guard_t> const &
  guards() const {
    return _guards;
  }

  inline std::vector<mitl2gta::transducer::transition_action_t> &actions() {
    return _actions;
  }

  inline std::vector<mitl2gta::transducer::transition_action_t> const &
  actions() const {
    return _actions;
  }

  inline mitl2gta::transducer::transition_on_t const &on() const { return _on; }

  inline std::vector<mitl2gta::transducer::gta_program_t> &gta_program() {
    return _gta_program;
  }

  inline std::vector<mitl2gta::transducer::gta_program_t> const &
  gta_program() const {
    return _gta_program;
  }

private:
  mitl2gta::transducer::state_id_t _from;
  mitl2gta::transducer::state_id_t _to;

  mitl2gta::transducer::transition_on_t _on;

  std::vector<mitl2gta::transducer::transition_guard_t> _guards;
  std::vector<mitl2gta::transducer::transition_action_t> _actions;

  std::vector<mitl2gta::transducer::gta_program_t> _gta_program;
};

} // namespace transducer
} // namespace mitl2gta
