/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>
#include <set>
#include <vector>

#include "mitl2gta/compile/truth_value_predictor.hpp"
#include "mitl2gta/gta_system/variable.hpp"
#include "mitl2gta/transducer/system.hpp"

namespace mitl2gta {

namespace memory {

enum mapping_mode_t {
  ONE_TO_ONE,
  OPTIMISED,
};

class memory_mapping_t {
public:
  memory_mapping_t(
      mitl2gta::memory::mapping_mode_t const mode,
      mitl2gta::transducer::system_t &system,
      mitl2gta::compilation::node_to_truth_value_predictor_t const
          &node_to_truth_value_predictor,
      std::map<mitl2gta::compilation::node_id_t,
               mitl2gta::memory::memory_id_t> const &placeholder_memory);

  std::string const &mapped_var(mitl2gta::memory::memory_id_t const id) const;

  inline std::vector<mitl2gta::gta::array_integer_variables_t> const &
  int_vars() const {
    return _int_vars;
  }

private:
  mitl2gta::memory::mapping_mode_t _mode;
  std::map<mitl2gta::memory::memory_id_t, std::string> _mem_to_var;

  std::vector<mitl2gta::gta::array_integer_variables_t> _int_vars;

  void
  one_to_one_mapping(std::set<mitl2gta::memory::memory_id_t> const &local_mems,
                     mitl2gta::boundval_t const local_lower,
                     mitl2gta::boundval_t const local_upper);

  void optimised_mapping(
      mitl2gta::transducer::system_t &system,
      std::set<mitl2gta::memory::memory_id_t> const &local_mems,
      mitl2gta::boundval_t const local_lower,
      mitl2gta::boundval_t const local_upper,
      mitl2gta::compilation::node_to_truth_value_predictor_t const
          &node_to_truth_value_predictor,
      std::map<mitl2gta::compilation::node_id_t,
               mitl2gta::memory::memory_id_t> const &placeholder_memory);
};

} // namespace memory
} // namespace mitl2gta
