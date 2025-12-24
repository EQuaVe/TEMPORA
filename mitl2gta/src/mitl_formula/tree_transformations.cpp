/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "mitl2gta/interval/interval.hpp"
#include "mitl2gta/mitl_formula/parsed_formula.hpp"
#include "mitl2gta/mitl_formula/tree_transformations.hpp"

namespace mitl2gta {

namespace mitl {

namespace {

class share_subformula_visitor_t final
    : public virtual mitl2gta::mitl::formula_visitor_t {
public:
  share_subformula_visitor_t() = default;
  ~share_subformula_visitor_t() = default;

  share_subformula_visitor_t(share_subformula_visitor_t const &) = delete;
  share_subformula_visitor_t(share_subformula_visitor_t &&) = default;
  share_subformula_visitor_t &
  operator=(share_subformula_visitor_t const &) = delete;
  share_subformula_visitor_t &
  operator=(share_subformula_visitor_t &&) = default;

  std::shared_ptr<mitl2gta::mitl::formula_t const> share_subformula(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr) {
    if (root_ptr.get() == nullptr) {
      throw std::invalid_argument("Null-pointer passed to subformula sharing");
    }

    _sym_table.clear();
    _transformed_last_exited_node = nullptr;

    root_ptr->visit(*this);
    assert(root_ptr->to_string() == _transformed_last_exited_node->to_string());

    return _transformed_last_exited_node;
  }

private:
  template <typename T> void visit_atomic(T const &formula) {
    auto const &&it = _sym_table.find(formula.to_string());
    if (it != _sym_table.end()) {
      _transformed_last_exited_node = it->second;
      return;
    }

    std::shared_ptr<T const> new_ptr = std::make_shared<T>(formula);
    _sym_table.insert({formula.to_string(), new_ptr});
    _transformed_last_exited_node = new_ptr;
    return;
  }

  template <typename T> void visit_unary_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::unary_operator_t, T>(),
                  "Must derive from unary_operator_t");

    auto const &&it = _sym_table.find(formula.to_string());
    if (it != _sym_table.end()) {
      _transformed_last_exited_node = it->second;
      return;
    }

    formula.subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> subformula =
        _transformed_last_exited_node;

    std::shared_ptr<T const> new_ptr = std::make_shared<T>(subformula);
    _sym_table.insert({formula.to_string(), new_ptr});
    _transformed_last_exited_node = new_ptr;
    return;
  }

  template <typename T> void visit_timed_unary_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::unary_operator_t, T>(),
                  "Must derive from unary_operator_t");
    static_assert(
        std::is_base_of<mitl2gta::mitl::timed_temporal_operator_t, T>(),
        "Must derive from unary_operator_t");

    auto const &&it = _sym_table.find(formula.to_string());
    if (it != _sym_table.end()) {
      _transformed_last_exited_node = it->second;
      return;
    }

    formula.subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> subformula =
        _transformed_last_exited_node;

    std::shared_ptr<T const> new_ptr =
        std::make_shared<T>(formula.interval(), subformula);
    _sym_table.insert({formula.to_string(), new_ptr});
    _transformed_last_exited_node = new_ptr;
    return;
  }

  template <typename T> void visit_binary_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::binary_operator_t, T>(),
                  "Must derive from binary_operator_t");

    auto const &&it = _sym_table.find(formula.to_string());
    if (it != _sym_table.end()) {
      _transformed_last_exited_node = it->second;
      return;
    }

    formula.left_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> left_subformula =
        _transformed_last_exited_node;

    formula.right_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> right_subformula =
        _transformed_last_exited_node;

    std::shared_ptr<T const> new_ptr =
        std::make_shared<T>(left_subformula, right_subformula);
    _sym_table.insert({formula.to_string(), new_ptr});
    _transformed_last_exited_node = new_ptr;
    return;
  }

  template <typename T> void visit_timed_binary_operator(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::binary_operator_t, T>(),
                  "Must derive from binary_operator_t");
    static_assert(
        std::is_base_of<mitl2gta::mitl::timed_temporal_operator_t, T>(),
        "Must derive from binary_operator_t");

    auto const &&it = _sym_table.find(formula.to_string());
    if (it != _sym_table.end()) {
      _transformed_last_exited_node = it->second;
      return;
    }

    formula.left_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> left_subformula =
        _transformed_last_exited_node;

    formula.right_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> right_subformula =
        _transformed_last_exited_node;

    std::shared_ptr<T const> new_ptr = std::make_shared<T>(
        formula.interval(), left_subformula, right_subformula);
    _sym_table.insert({formula.to_string(), new_ptr});
    _transformed_last_exited_node = new_ptr;
    return;
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
    visit_unary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::or_t const &formula) override {
    visit_binary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::and_t const &formula) override {
    visit_binary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_next_t const &formula) override {
    visit_unary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_next_t const &formula) override {
    visit_timed_unary_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_yesterday_t const &formula) override {
    visit_unary_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::timed_yesterday_t const &formula) override {
    visit_timed_unary_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_finally_t const &formula) override {
    visit_unary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_finally_t const &formula) override {
    visit_timed_unary_operator(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_globally_t const &formula) override {
    visit_unary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_globally_t const &formula) override {
    visit_timed_unary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_until_t const &formula) override {
    visit_binary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_until_t const &formula) override {
    visit_timed_binary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_since_t const &formula) override {
    visit_binary_operator(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_since_t const &formula) override {
    visit_timed_binary_operator(formula);
  }

  std::map<std::string const, std::shared_ptr<mitl2gta::mitl::formula_t const>>
      _sym_table;

  std::shared_ptr<mitl2gta::mitl::formula_t const>
      _transformed_last_exited_node;
};

class remove_unbounded_intervals_visitor_t final
    : public mitl2gta::mitl::formula_visitor_t {
public:
  remove_unbounded_intervals_visitor_t() = default;
  remove_unbounded_intervals_visitor_t(
      remove_unbounded_intervals_visitor_t const &) = delete;
  remove_unbounded_intervals_visitor_t(
      remove_unbounded_intervals_visitor_t &&) = default;
  remove_unbounded_intervals_visitor_t &
  operator=(remove_unbounded_intervals_visitor_t const &) = delete;
  remove_unbounded_intervals_visitor_t &
  operator=(remove_unbounded_intervals_visitor_t &&) = default;

  ~remove_unbounded_intervals_visitor_t() = default;

  std::shared_ptr<mitl2gta::mitl::formula_t const> remove_unbounded_intervals(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr) {
    if (root_ptr.get() == nullptr) {
      throw std::invalid_argument(
          "Null pointer passed to remove_unbounded_intervals");
    }

    _last_exited_node = nullptr;
    root_ptr->visit(*this);
    return _last_exited_node;
  }

private:
  template <typename Timed, typename UnTimed>
  void visit_timed_unary_operator(Timed const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::unary_operator_t, Timed>(),
                  "Must derive from unary_operator_t");
    static_assert(
        std::is_base_of<mitl2gta::mitl::timed_temporal_operator_t, Timed>(),
        "Must derive from timed_temporal_operator_t");

    formula.subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> subformula =
        _last_exited_node;

    if (mitl2gta::interval_type(formula.interval()) ==
        mitl2gta::interval_type_t::UNBOUNDED) {
      _last_exited_node = std::make_shared<UnTimed>(subformula);
      return;
    }

    _last_exited_node = std::make_shared<Timed>(formula.interval(), subformula);
    return;
  }

  template <typename Timed, typename UnTimed>
  void visit_timed_binary_operator(Timed const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::binary_operator_t, Timed>(),
                  "Must derive from binary_operator_t");
    static_assert(
        std::is_base_of<mitl2gta::mitl::timed_temporal_operator_t, Timed>(),
        "Must derive from timed_temporal_operator_t");

    formula.left_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> left_subformula =
        _last_exited_node;

    formula.right_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> right_subformula =
        _last_exited_node;

    if (mitl2gta::interval_type(formula.interval()) ==
        mitl2gta::interval_type_t::UNBOUNDED) {
      _last_exited_node =
          std::make_shared<UnTimed>(left_subformula, right_subformula);
      return;
    }

    _last_exited_node = std::make_shared<Timed>(
        formula.interval(), left_subformula, right_subformula);
    return;
  }

  template <typename T> void visit_node_without_interval(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::formula_t, T>(),
                  "Must derive from formula_t");
    _last_exited_node = std::make_shared<T>(formula);
    return;
  }

  template <typename T> void visit_unary_untimed_node(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::formula_t, T>(),
                  "Must derive from formula_t");
    static_assert(std::is_base_of<mitl2gta::mitl::unary_operator_t, T>(),
                  "Must derive from unary_operator_t");

    formula.subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> subformula =
        _last_exited_node;

    _last_exited_node = std::make_shared<T>(subformula);
    return;
  }

  template <typename T> void visit_binary_untimed_node(T const &formula) {
    static_assert(std::is_base_of<mitl2gta::mitl::formula_t, T>(),
                  "Must derive from formula_t");
    static_assert(std::is_base_of<mitl2gta::mitl::binary_operator_t, T>(),
                  "Must derive from binary_operator_t");

    formula.left_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> left_subformula =
        _last_exited_node;

    formula.right_subformula()->visit(*this);
    std::shared_ptr<mitl2gta::mitl::formula_t const> right_subformula =
        _last_exited_node;

    _last_exited_node = std::make_shared<T>(left_subformula, right_subformula);
    return;
  }

  virtual void
  visit(mitl2gta::mitl::atomic_proposition_t const &formula) override {
    visit_node_without_interval(formula);
  }

  virtual void
  visit(mitl2gta::mitl::atomic_constant_t const &formula) override {
    visit_node_without_interval(formula);
  }

  virtual void visit(mitl2gta::mitl::negation_t const &formula) override {
    visit_unary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::or_t const &formula) override {
    visit_binary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::and_t const &formula) override {
    visit_binary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_next_t const &formula) override {
    visit_unary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_next_t const &formula) override {
    visit_timed_unary_operator<mitl2gta::mitl::timed_next_t,
                               mitl2gta::mitl::untimed_next_t>(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_yesterday_t const &formula) override {
    visit_unary_untimed_node(formula);
  }

  virtual void
  visit(mitl2gta::mitl::timed_yesterday_t const &formula) override {
    visit_timed_unary_operator<mitl2gta::mitl::timed_yesterday_t,
                               mitl2gta::mitl::untimed_yesterday_t>(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_finally_t const &formula) override {
    visit_unary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_finally_t const &formula) override {
    visit_timed_unary_operator<mitl2gta::mitl::timed_finally_t,
                               mitl2gta::mitl::untimed_finally_t>(formula);
  }

  virtual void
  visit(mitl2gta::mitl::untimed_globally_t const &formula) override {
    visit_unary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_globally_t const &formula) override {
    visit_timed_unary_operator<mitl2gta::mitl::timed_globally_t,
                               mitl2gta::mitl::untimed_globally_t>(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_until_t const &formula) override {
    visit_binary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_until_t const &formula) override {
    visit_timed_binary_operator<mitl2gta::mitl::timed_until_t,
                                mitl2gta::mitl::untimed_until_t>(formula);
  }

  virtual void visit(mitl2gta::mitl::untimed_since_t const &formula) override {
    visit_binary_untimed_node(formula);
  }

  virtual void visit(mitl2gta::mitl::timed_since_t const &formula) override {
    visit_timed_binary_operator<mitl2gta::mitl::timed_since_t,
                                mitl2gta::mitl::untimed_since_t>(formula);
  }

  std::shared_ptr<mitl2gta::mitl::formula_t const> _last_exited_node;
};

} // namespace

std::shared_ptr<mitl2gta::mitl::formula_t const> share_subformula(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr) {
  mitl2gta::mitl::share_subformula_visitor_t share_visitor;
  return share_visitor.share_subformula(root_ptr);
}

std::shared_ptr<mitl2gta::mitl::formula_t const> remove_unbounded_intervals(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr) {
  mitl2gta::mitl::remove_unbounded_intervals_visitor_t simplify_visitor;
  return simplify_visitor.remove_unbounded_intervals(root_ptr);
}

} // namespace mitl

} // namespace mitl2gta
