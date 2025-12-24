/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <set>

#include "mitl2gta/compile/id_allocation.hpp"
#include "mitl2gta/compile/truth_value_predictor.hpp"
#include "mitl2gta/interval/bound.hpp"
#include "mitl2gta/interval/interval.hpp"
#include "mitl2gta/memory/memory_mapping.hpp"

namespace mitl2gta {

namespace compilation {

enum node_type_t {
  ATOMIC_PROPOSITION,
  ATOMIC_CONSTANT,
  BOOLEAN_OPERATOR,
};

enum verification_mode_t {
  SATISFIABILITY,
  MODEL_CHECKING,
};

struct translation_specs_t {
  std::set<enum mitl2gta::compilation::node_type_t> translate_for_types;
  enum mitl2gta::transducer::system_for_t system_for;
  mitl2gta::memory::mapping_mode_t mapping_mode;
};

class compiled_node_t {
public:
  explicit compiled_node_t(
      mitl2gta::compilation::compiled_node_id_allocator_t &allocator);

  compiled_node_t(compiled_node_t const &) = default;
  compiled_node_t(compiled_node_t &&) = default;
  compiled_node_t &operator=(compiled_node_t const &) = default;
  compiled_node_t &operator=(compiled_node_t &&) = default;

  inline mitl2gta::compilation::node_id_t id() const { return _id; }

  virtual mitl2gta::compilation::truth_value_predictor_t
  generate_truth_value_predictor(
      mitl2gta::transducer::transducer_construction_tools_t &tools,
      mitl2gta::compilation::translation_specs_t const &specs) const = 0;

  virtual std::set<mitl2gta::compilation::node_id_t>
  direct_dependencies() const = 0;

protected:
  mitl2gta::compilation::node_id_t _id;
};

class unary_node_t {
public:
  unary_node_t(mitl2gta::compilation::node_id_t child);

  unary_node_t(unary_node_t const &) = default;
  unary_node_t(unary_node_t &&) = default;
  unary_node_t &operator=(unary_node_t const &) = default;
  unary_node_t &operator=(unary_node_t &&) = default;

  inline mitl2gta::compilation::node_id_t child() const { return _child; }

protected:
  mitl2gta::compilation::node_id_t _child;
};

class binary_node_t {
public:
  binary_node_t(mitl2gta::compilation::node_id_t lchild,
                mitl2gta::compilation::node_id_t rchild);

  binary_node_t(binary_node_t const &) = default;
  binary_node_t(binary_node_t &&) = default;
  binary_node_t &operator=(binary_node_t const &) = default;
  binary_node_t &operator=(binary_node_t &&) = default;

  inline mitl2gta::compilation::node_id_t lchild() const { return _lchild; }
  inline mitl2gta::compilation::node_id_t rchild() const { return _rchild; }

protected:
  mitl2gta::compilation::node_id_t _lchild;
  mitl2gta::compilation::node_id_t _rchild;
};

class timed_node_t {
public:
  timed_node_t(mitl2gta::nonempty_interval_t const interval);

  timed_node_t(timed_node_t const &) = default;
  timed_node_t(timed_node_t &&) = default;
  timed_node_t &operator=(timed_node_t const &) = default;
  timed_node_t &operator=(timed_node_t &&) = default;

  inline mitl2gta::nonempty_interval_t interval() const { return _interval; }

protected:
  mitl2gta::nonempty_interval_t _interval;
};

class upper_bounded_timed_node_t : public mitl2gta::compilation::timed_node_t {
public:
  upper_bounded_timed_node_t(mitl2gta::interval_bound_t const upper_bound);

  upper_bounded_timed_node_t(upper_bounded_timed_node_t const &) = default;
  upper_bounded_timed_node_t(upper_bounded_timed_node_t &&) = default;
  upper_bounded_timed_node_t &
  operator=(upper_bounded_timed_node_t const &) = default;
  upper_bounded_timed_node_t &
  operator=(upper_bounded_timed_node_t &&) = default;

  inline mitl2gta::interval_bound_t upper_bound() const {
    return interval().upper_bound();
  }
};

class lower_bounded_timed_node_t : public mitl2gta::compilation::timed_node_t {
public:
  lower_bounded_timed_node_t(mitl2gta::interval_bound_t const lower_bound);

  lower_bounded_timed_node_t(lower_bounded_timed_node_t const &) = default;
  lower_bounded_timed_node_t(lower_bounded_timed_node_t &&) = default;
  lower_bounded_timed_node_t &
  operator=(lower_bounded_timed_node_t const &) = default;
  lower_bounded_timed_node_t &
  operator=(lower_bounded_timed_node_t &&) = default;

  inline mitl2gta::interval_bound_t lower_bound() const {
    return interval().lower_bound();
  }
};

} // namespace compilation
} // namespace mitl2gta
