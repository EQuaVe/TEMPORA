/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/atomic_constant.hpp"

namespace mitl2gta {

namespace compilation {

compiled_atomic_constant_t::compiled_atomic_constant_t(
    enum mitl2gta::mitl::constant_value_t const val,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator), _val(val) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_atomic_constant_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  if (specs.translate_for_types.find(
          mitl2gta::compilation::node_type_t::ATOMIC_CONSTANT) ==
      specs.translate_for_types.end()) {
    return mitl2gta::compilation::atomic_constant_t{_val};
  }

  mitl2gta::transducer::node_value_t node_value;

  switch (_val) {
  case mitl2gta::mitl::constant_value_t::BOOL_TRUE: {
    node_value = mitl2gta::transducer::node_value_t::TRUE;
    break;
  }
  case mitl2gta::mitl::constant_value_t::BOOL_FALSE: {
    node_value = mitl2gta::transducer::node_value_t::FALSE;
    break;
  }
  };

  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), mitl2gta::transducer::on_node_values_t(), {},
      {mitl2gta::transducer::set_node_value_t{_id, node_value}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1}};

  return (mitl2gta::compilation::predicted_by_transducer_t{t});
}

std::set<mitl2gta::compilation::node_id_t>
compiled_atomic_constant_t::direct_dependencies() const {
  return {};
}

} // namespace compilation

} // namespace mitl2gta
