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

class compiled_negation_t final
    : public mitl2gta::compilation::unary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_negation_t(
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_negation_t(compiled_negation_t const &) = default;
  compiled_negation_t(compiled_negation_t &&) = default;
  compiled_negation_t &operator=(compiled_negation_t const &) = default;
  compiled_negation_t &operator=(compiled_negation_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_or_t final
    : public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_or_t(mitl2gta::compilation::node_id_t lchild,
                mitl2gta::compilation::node_id_t rchild,
                mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_or_t(compiled_or_t const &) = default;
  compiled_or_t(compiled_or_t &&) = default;
  compiled_or_t &operator=(compiled_or_t const &) = default;
  compiled_or_t &operator=(compiled_or_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_and_t final
    : public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_and_t(
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_and_t(compiled_and_t const &) = default;
  compiled_and_t(compiled_and_t &&) = default;
  compiled_and_t &operator=(compiled_and_t const &) = delete;
  compiled_and_t &operator=(compiled_and_t &&) = delete;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

} // namespace compilation
} // namespace mitl2gta
