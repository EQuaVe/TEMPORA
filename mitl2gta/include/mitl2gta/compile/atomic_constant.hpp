/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/mitl_formula/parsed_formula.hpp"

namespace mitl2gta {
namespace compilation {

class compiled_atomic_constant_t final
    : public virtual mitl2gta::compilation::compiled_node_t {

public:
  compiled_atomic_constant_t(
      enum mitl2gta::mitl::constant_value_t const val,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  inline mitl2gta::mitl::constant_value_t val() const { return _val; }

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;

private:
  enum mitl2gta::mitl::constant_value_t _val;
};

} // namespace compilation
} // namespace mitl2gta
