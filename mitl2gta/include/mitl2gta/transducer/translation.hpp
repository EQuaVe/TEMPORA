/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/gta_system/system.hpp"
#include "mitl2gta/memory/memory_mapping.hpp"

namespace mitl2gta {

namespace transducer {

class synchronized_gta_system_t {
public:
  synchronized_gta_system_t(
      mitl2gta::transducer::system_t &system,
      std::map<mitl2gta::compilation::node_id_t,
               mitl2gta::compilation::truth_value_predictor_t> const
          &node_to_truth_value_predictor,
      std::map<mitl2gta::compilation::node_id_t,
               mitl2gta::memory::memory_id_t> const &placeholder_memory,
      mitl2gta::memory::memory_mapping_t const &memory_mapping,
      mitl2gta::compilation::verification_mode_t const verification_mode);

  mitl2gta::gta::system_t gta_system() const;

private:
  using transducer_idx_t = std::size_t;

  mitl2gta::gta::location_t
  to_gta_loc(mitl2gta::transducer::state_t const &state,
             std::string const &label);

  std::vector<mitl2gta::gta::edge_t>
  to_gta_edge(mitl2gta::transducer::edge_t const &edge,
              transducer_idx_t const idx);

  mitl2gta::gta::process_t
  to_gta_proc(mitl2gta::transducer::transducer_t const &transducer,
              transducer_idx_t const idx);

  void add_controller_with_sync();

  mitl2gta::transducer::system_t &_system;

  std::map<mitl2gta::compilation::node_id_t,
           mitl2gta::compilation::truth_value_predictor_t> const
      &_node_to_truth_value_predictor;

  std::map<mitl2gta::compilation::node_id_t,
           mitl2gta::memory::memory_id_t> const &_placeholder_memory;

  mitl2gta::memory::memory_mapping_t const &_memory_mapping;

  mitl2gta::compilation::verification_mode_t const _verification_mode;

  std::set<std::string> _events;

  std::set<std::string> _labels;

  std::vector<mitl2gta::gta::process_t> _procs;

  std::vector<mitl2gta::gta::sync_constraints> _sync_constraints;
};

} // namespace transducer

} // namespace mitl2gta
