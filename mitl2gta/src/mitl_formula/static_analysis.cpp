/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <type_traits>

#include "mitl2gta/mitl_formula/parsed_formula.hpp"
#include "mitl2gta/mitl_formula/static_analysis.hpp"

namespace mitl2gta {

namespace mitl {

namespace {

class node_placement_compute_t final
    : public virtual mitl2gta::mitl::formula_visitor_t {
public:
  node_placement_compute_t() {}

  node_placement_compute_t(node_placement_compute_t const &) = delete;
  node_placement_compute_t(node_placement_compute_t &&) = delete;
  node_placement_compute_t &
  operator=(node_placement_compute_t const &) = delete;
  node_placement_compute_t &operator=(node_placement_compute_t &&) = delete;

  mitl2gta::mitl::subformula_placement_map_t compute_node_placements(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr) {
    if (root_ptr.get() == nullptr) {
      throw std::invalid_argument("Null-pointer passed to subformula sharing");
    }

    _nested.clear();
    _top_level.clear();
    _context = EXPLORING_TOP_LEVEL;

    root_ptr->visit(*this);
    mitl2gta::mitl::subformula_placement_map_t placement_map;

    for (auto const &n : _top_level) {
      if (_nested.find(n) != _nested.end()) {
        placement_map.insert(
            {n, mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED});
      } else {
        placement_map.insert(
            {n, mitl2gta::mitl::subformula_placement_t::TOP_LEVEL});
      }
    }

    for (auto const &n : _nested) {
      if (_top_level.find(n) != _top_level.end()) {
        continue;
      }

      placement_map.insert({n, mitl2gta::mitl::subformula_placement_t::NESTED});
    }

    return placement_map;
  }

private:
  enum node_placecement_context_t {
    EXPLORING_TOP_LEVEL,
    EXPLORING_NESTED,
  };

  bool add_subformula(mitl2gta::mitl::formula_t const &formula) {
    switch (_context) {
    case EXPLORING_TOP_LEVEL: {
      return _top_level.insert(&formula).second;
    }

    case EXPLORING_NESTED: {
      return _nested.insert(&formula).second;
    }
    }
    throw std::runtime_error("Incomplete switch statement");
  };

  template <typename T> void visit_atomic(T const &formula) {
    add_subformula(formula);
  }

  template <typename T> void visit_unary_boolean_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::unary_operator_t, T>(),
                  "Must derive from unary_operator_t");
    if (add_subformula(formula)) {
      formula.subformula()->visit(*this);
    }
  }

  template <typename T> void visit_binary_boolean_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::binary_operator_t, T>(),
                  "Must derive from binary_operator_t");
    if (add_subformula(formula)) {
      formula.left_subformula()->visit(*this);
      formula.right_subformula()->visit(*this);
    }
  }

  template <typename T> void visit_unary_temporal_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::unary_operator_t, T>(),
                  "Must derive from unary_operator_t");
    if (add_subformula(formula)) {
      node_placecement_context_t prev_ctx = _context;
      _context = EXPLORING_NESTED;
      formula.subformula()->visit(*this);
      _context = prev_ctx;
    }
  }

  template <typename T> void visit_binary_temporal_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::binary_operator_t, T>(),
                  "Must derive from binary_operator_t");
    if (add_subformula(formula)) {
      node_placecement_context_t prev_ctx = _context;
      _context = EXPLORING_NESTED;
      formula.left_subformula()->visit(*this);
      formula.right_subformula()->visit(*this);
      _context = prev_ctx;
    }
  }

  virtual void
  visit(mitl2gta::mitl::atomic_proposition_t const &formula) override {
    visit_atomic(formula);
  }

  virtual void
  visit(mitl2gta::mitl::atomic_constant_t const &formula) override {
    visit_atomic(formula);
  }

  virtual void visit(mitl2gta::mitl::negation_t const &formula) override {
    visit_unary_boolean_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::or_t const &formula) override {
    visit_binary_boolean_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::and_t const &formula) override {
    visit_binary_boolean_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_next_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_next_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_yesterday_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::timed_yesterday_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_finally_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_finally_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_globally_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_globally_t const &formula) override {
    visit_unary_temporal_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_until_t const &formula) override {
    visit_binary_temporal_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_until_t const &formula) override {
    visit_binary_temporal_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_since_t const &formula) override {
    visit_binary_temporal_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_since_t const &formula) override {
    visit_binary_temporal_operator(formula);
  }

  node_placecement_context_t _context;

  std::set<mitl2gta::mitl::formula_t const *> _top_level;
  std::set<mitl2gta::mitl::formula_t const *> _nested;
};

} // namespace

mitl2gta::mitl::subformula_placement_map_t compute_node_placements(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr) {
  node_placement_compute_t computer;
  return computer.compute_node_placements(root_ptr);
}

} // namespace mitl

} // namespace mitl2gta
