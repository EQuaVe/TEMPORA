/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/compile/compiled_node.hpp"

namespace mitl2gta {
namespace compilation {

class compiled_upper_bounded_finally_t
    : public mitl2gta::compilation::unary_node_t,
      public mitl2gta::compilation::upper_bounded_timed_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_upper_bounded_finally_t(
      mitl2gta::interval_bound_t const upper_bound,
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_upper_bounded_finally_t(compiled_upper_bounded_finally_t const &) =
      default;
  compiled_upper_bounded_finally_t(compiled_upper_bounded_finally_t &&) =
      default;
  compiled_upper_bounded_finally_t &
  operator=(compiled_upper_bounded_finally_t const &) = default;
  compiled_upper_bounded_finally_t &
  operator=(compiled_upper_bounded_finally_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

} // namespace compilation

} // namespace mitl2gta
