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

class compiled_toplevel_node_value_t final
    : public mitl2gta::compilation::unary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_toplevel_node_value_t(
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_toplevel_node_value_t(compiled_toplevel_node_value_t const &) =
      default;
  compiled_toplevel_node_value_t(compiled_toplevel_node_value_t &&) = default;
  compiled_toplevel_node_value_t &
  operator=(compiled_toplevel_node_value_t const &) = default;
  compiled_toplevel_node_value_t &
  operator=(compiled_toplevel_node_value_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_toplevel_negation_t final
    : public mitl2gta::compilation::unary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_toplevel_negation_t(
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_toplevel_negation_t(compiled_toplevel_negation_t const &) = default;
  compiled_toplevel_negation_t(compiled_toplevel_negation_t &&) = default;
  compiled_toplevel_negation_t &
  operator=(compiled_toplevel_negation_t const &) = default;
  compiled_toplevel_negation_t &
  operator=(compiled_toplevel_negation_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_toplevel_or_t final
    : public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_toplevel_or_t(
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_toplevel_or_t(compiled_toplevel_or_t const &) = default;
  compiled_toplevel_or_t(compiled_toplevel_or_t &&) = default;
  compiled_toplevel_or_t &operator=(compiled_toplevel_or_t const &) = default;
  compiled_toplevel_or_t &operator=(compiled_toplevel_or_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_toplevel_and_t final
    : public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_toplevel_and_t(
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_toplevel_and_t(compiled_toplevel_and_t const &) = default;
  compiled_toplevel_and_t(compiled_toplevel_and_t &&) = default;
  compiled_toplevel_and_t &operator=(compiled_toplevel_and_t const &) = default;
  compiled_toplevel_and_t &operator=(compiled_toplevel_and_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_untimed_toplevel_next_t final
    : public mitl2gta::compilation::unary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_untimed_toplevel_next_t(
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_untimed_toplevel_next_t(compiled_untimed_toplevel_next_t const &) =
      default;
  compiled_untimed_toplevel_next_t(compiled_untimed_toplevel_next_t &&) =
      default;
  compiled_untimed_toplevel_next_t &
  operator=(compiled_untimed_toplevel_next_t const &) = default;
  compiled_untimed_toplevel_next_t &
  operator=(compiled_untimed_toplevel_next_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_timed_toplevel_next_t final
    : public mitl2gta::compilation::unary_node_t,
      public mitl2gta::compilation::timed_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_timed_toplevel_next_t(
      mitl2gta::nonempty_interval_t const interval,
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_timed_toplevel_next_t(compiled_timed_toplevel_next_t const &) =
      default;
  compiled_timed_toplevel_next_t(compiled_timed_toplevel_next_t &&) = default;
  compiled_timed_toplevel_next_t &
  operator=(compiled_timed_toplevel_next_t const &) = default;
  compiled_timed_toplevel_next_t &
  operator=(compiled_timed_toplevel_next_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_untimed_toplevel_until_t final
    : public mitl2gta::compilation::binary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_untimed_toplevel_until_t(
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_untimed_toplevel_until_t(compiled_untimed_toplevel_until_t const &) =
      default;
  compiled_untimed_toplevel_until_t(compiled_untimed_toplevel_until_t &&) =
      default;
  compiled_untimed_toplevel_until_t &
  operator=(compiled_untimed_toplevel_until_t const &) = default;
  compiled_untimed_toplevel_until_t &
  operator=(compiled_untimed_toplevel_until_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_timed_toplevel_until_t final
    : public mitl2gta::compilation::binary_node_t,
      public mitl2gta::compilation::timed_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_timed_toplevel_until_t(
      mitl2gta::nonempty_interval_t const interval,
      mitl2gta::compilation::node_id_t lchild,
      mitl2gta::compilation::node_id_t rchild,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_timed_toplevel_until_t(compiled_timed_toplevel_until_t const &) =
      default;
  compiled_timed_toplevel_until_t(compiled_timed_toplevel_until_t &&) = default;
  compiled_timed_toplevel_until_t &
  operator=(compiled_timed_toplevel_until_t const &) = default;
  compiled_timed_toplevel_until_t &
  operator=(compiled_timed_toplevel_until_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_untimed_toplevel_finally_t final
    : public mitl2gta::compilation::unary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_untimed_toplevel_finally_t(
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_untimed_toplevel_finally_t(
      compiled_untimed_toplevel_finally_t const &) = default;
  compiled_untimed_toplevel_finally_t(compiled_untimed_toplevel_finally_t &&) =
      default;
  compiled_untimed_toplevel_finally_t &
  operator=(compiled_untimed_toplevel_finally_t const &) = default;
  compiled_untimed_toplevel_finally_t &
  operator=(compiled_untimed_toplevel_finally_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_timed_toplevel_finally_t final
    : public mitl2gta::compilation::unary_node_t,
      public mitl2gta::compilation::timed_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_timed_toplevel_finally_t(
      mitl2gta::nonempty_interval_t const interval,
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_timed_toplevel_finally_t(compiled_timed_toplevel_finally_t const &) =
      default;
  compiled_timed_toplevel_finally_t(compiled_timed_toplevel_finally_t &&) =
      default;
  compiled_timed_toplevel_finally_t &
  operator=(compiled_timed_toplevel_finally_t const &) = default;
  compiled_timed_toplevel_finally_t &
  operator=(compiled_timed_toplevel_finally_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_untimed_toplevel_globally_t final
    : public mitl2gta::compilation::unary_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_untimed_toplevel_globally_t(
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_untimed_toplevel_globally_t(
      compiled_untimed_toplevel_globally_t const &) = default;
  compiled_untimed_toplevel_globally_t(
      compiled_untimed_toplevel_globally_t &&) = default;
  compiled_untimed_toplevel_globally_t &
  operator=(compiled_untimed_toplevel_globally_t const &) = default;
  compiled_untimed_toplevel_globally_t &
  operator=(compiled_untimed_toplevel_globally_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

class compiled_timed_toplevel_globally_t final
    : public mitl2gta::compilation::unary_node_t,
      public mitl2gta::compilation::timed_node_t,
      public virtual mitl2gta::compilation::compiled_node_t {
public:
  compiled_timed_toplevel_globally_t(
      mitl2gta::nonempty_interval_t const interval,
      mitl2gta::compilation::node_id_t child,
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_timed_toplevel_globally_t(
      compiled_timed_toplevel_globally_t const &) = default;
  compiled_timed_toplevel_globally_t(compiled_timed_toplevel_globally_t &&) =
      default;
  compiled_timed_toplevel_globally_t &
  operator=(compiled_timed_toplevel_globally_t const &) = default;
  compiled_timed_toplevel_globally_t &
  operator=(compiled_timed_toplevel_globally_t &&) = default;

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const override;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const override;
};

} // namespace compilation
} // namespace mitl2gta
