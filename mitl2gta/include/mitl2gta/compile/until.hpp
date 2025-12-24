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

class compiled_untimed_until_t
    : public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_untimed_until_t(
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_untimed_until_t(compiled_untimed_until_t const &) = default;
  compiled_untimed_until_t(compiled_untimed_until_t &&) = default;
  compiled_untimed_until_t &
  operator=(compiled_untimed_until_t const &) = default;
  compiled_untimed_until_t &operator=(compiled_untimed_until_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_timed_until_t
    : public mitl2gta::compilation::timed_node_t,
      public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_timed_until_t(
      mitl2gta::nonempty_interval_t const interval,
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_timed_until_t(compiled_timed_until_t const &) = default;
  compiled_timed_until_t(compiled_timed_until_t &&) = default;
  compiled_timed_until_t &operator=(compiled_timed_until_t const &) = default;
  compiled_timed_until_t &operator=(compiled_timed_until_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_upper_bounded_until_t
    : public mitl2gta::compilation::upper_bounded_timed_node_t,
      public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_upper_bounded_until_t(
      mitl2gta::interval_bound_t const upper_bound,
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_upper_bounded_until_t(compiled_upper_bounded_until_t const &) =
      default;
  compiled_upper_bounded_until_t(compiled_upper_bounded_until_t &&) = default;
  compiled_upper_bounded_until_t &
  operator=(compiled_upper_bounded_until_t const &) = default;
  compiled_upper_bounded_until_t &
  operator=(compiled_upper_bounded_until_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;
  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_lower_bounded_until_t
    : public mitl2gta::compilation::lower_bounded_timed_node_t,
      public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_lower_bounded_until_t(
      mitl2gta::interval_bound_t const lower_bound,
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_lower_bounded_until_t(compiled_lower_bounded_until_t const &) =
      default;
  compiled_lower_bounded_until_t(compiled_lower_bounded_until_t &&) = default;
  compiled_lower_bounded_until_t &
  operator=(compiled_lower_bounded_until_t const &) = default;
  compiled_lower_bounded_until_t &
  operator=(compiled_lower_bounded_until_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

} // namespace compilation

} // namespace mitl2gta
