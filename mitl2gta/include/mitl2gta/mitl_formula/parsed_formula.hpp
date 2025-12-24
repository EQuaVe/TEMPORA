/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <memory>
#include <ostream>

#include "mitl2gta/interval/interval.hpp"

namespace mitl2gta {

namespace mitl {

class formula_visitor_t;

class formula_t {
public:
  virtual void visit(mitl2gta::mitl::formula_visitor_t &visitor) const = 0;
  virtual std::ostream &output(std::ostream &os) const = 0;

  std::string to_string() const;
};

inline std::ostream &operator<<(std::ostream &os, formula_t const &formula) {
  return formula.output(os);
}

class atomic_proposition_t final : public virtual mitl2gta::mitl::formula_t {
public:
  explicit atomic_proposition_t(std::string const &name);

  atomic_proposition_t(atomic_proposition_t const &) = default;
  atomic_proposition_t(atomic_proposition_t &&) = delete;
  atomic_proposition_t &operator=(atomic_proposition_t const &) = delete;
  atomic_proposition_t &operator=(atomic_proposition_t &&) = delete;

  ~atomic_proposition_t() = default;

  inline std::string name() const { return _name; }

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;

  std::ostream &output(std::ostream &os) const override;

private:
  std::string const _name;
};

enum constant_value_t {
  BOOL_TRUE,
  BOOL_FALSE,
};

std::ostream &operator<<(std::ostream &os, enum constant_value_t const value);

class atomic_constant_t final : public virtual mitl2gta::mitl::formula_t {
public:
  explicit atomic_constant_t(enum mitl2gta::mitl::constant_value_t const value);

  atomic_constant_t(atomic_constant_t const &) = default;
  atomic_constant_t(atomic_constant_t &&) = delete;
  atomic_constant_t &operator=(atomic_constant_t const &) = delete;
  atomic_constant_t &operator=(atomic_constant_t &&) = delete;

  ~atomic_constant_t() = default;

  inline enum mitl2gta::mitl::constant_value_t value() const { return _value; }

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;

  std::ostream &output(std::ostream &os) const override;

private:
  enum constant_value_t const _value;
};

class unary_operator_t : public virtual mitl2gta::mitl::formula_t {
public:
  explicit unary_operator_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  ~unary_operator_t() = default;

  inline std::shared_ptr<mitl2gta::mitl::formula_t const> subformula() const {
    return _subformula;
  }

protected:
  std::shared_ptr<mitl2gta::mitl::formula_t const> const _subformula;
};

class negation_t final : public virtual mitl2gta::mitl::unary_operator_t {
public:
  explicit negation_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  negation_t(negation_t const &) = default;
  negation_t(negation_t &&) = default;
  negation_t &operator=(negation_t const &) = delete;
  negation_t &operator=(negation_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class binary_operator_t : public virtual mitl2gta::mitl::formula_t {
public:
  binary_operator_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula);

  ~binary_operator_t() = default;

  inline std::shared_ptr<mitl2gta::mitl::formula_t const>
  left_subformula() const {
    return _left_subformula;
  }

  inline std::shared_ptr<mitl2gta::mitl::formula_t const>
  right_subformula() const {
    return _right_subformula;
  }

protected:
  std::shared_ptr<mitl2gta::mitl::formula_t const> const _left_subformula;
  std::shared_ptr<mitl2gta::mitl::formula_t const> const _right_subformula;
};

class or_t final : public virtual mitl2gta::mitl::binary_operator_t {
public:
  or_t(std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
       std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula);

  or_t(or_t const &) = default;
  or_t(or_t &&) = default;
  or_t &operator=(or_t const &) = delete;
  or_t &operator=(or_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class and_t final : public virtual mitl2gta::mitl::binary_operator_t {
public:
  and_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula);

  and_t(and_t const &) = default;
  and_t(and_t &&) = default;
  and_t &operator=(and_t const &) = delete;
  and_t &operator=(and_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class timed_temporal_operator_t : public virtual mitl2gta::mitl::formula_t {
public:
  explicit timed_temporal_operator_t(
      mitl2gta::nonempty_interval_t const &interval);

  ~timed_temporal_operator_t() = default;

  inline mitl2gta::nonempty_interval_t const &interval() const {
    return _interval;
  }

protected:
  mitl2gta::nonempty_interval_t const _interval;
};

class untimed_next_t final : public virtual mitl2gta::mitl::unary_operator_t {
public:
  explicit untimed_next_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  untimed_next_t(untimed_next_t const &) = default;
  untimed_next_t(untimed_next_t &&) = default;
  untimed_next_t &operator=(untimed_next_t const &) = delete;
  untimed_next_t &operator=(untimed_next_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class timed_next_t final
    : public virtual mitl2gta::mitl::timed_temporal_operator_t,
      public virtual mitl2gta::mitl::unary_operator_t {
public:
  timed_next_t(
      mitl2gta::nonempty_interval_t const &interval,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  timed_next_t(timed_next_t const &) = default;
  timed_next_t(timed_next_t &&) = default;
  timed_next_t &operator=(timed_next_t const &) = delete;
  timed_next_t &operator=(timed_next_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class untimed_yesterday_t final
    : public virtual mitl2gta::mitl::unary_operator_t {
public:
  explicit untimed_yesterday_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  untimed_yesterday_t(untimed_yesterday_t const &) = default;
  untimed_yesterday_t(untimed_yesterday_t &&) = default;
  untimed_yesterday_t &operator=(untimed_yesterday_t const &) = delete;
  untimed_yesterday_t &operator=(untimed_yesterday_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class timed_yesterday_t final
    : public virtual mitl2gta::mitl::timed_temporal_operator_t,
      public virtual mitl2gta::mitl::unary_operator_t {
public:
  timed_yesterday_t(
      mitl2gta::nonempty_interval_t const &interval,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  timed_yesterday_t(timed_yesterday_t const &) = default;
  timed_yesterday_t(timed_yesterday_t &&) = default;
  timed_yesterday_t &operator=(timed_yesterday_t const &) = delete;
  timed_yesterday_t &operator=(timed_yesterday_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class untimed_finally_t final
    : public virtual mitl2gta::mitl::unary_operator_t {
public:
  explicit untimed_finally_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  untimed_finally_t(untimed_finally_t const &) = default;
  untimed_finally_t(untimed_finally_t &&) = default;
  untimed_finally_t &operator=(untimed_finally_t const &) = delete;
  untimed_finally_t &operator=(untimed_finally_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class timed_finally_t final
    : public virtual mitl2gta::mitl::timed_temporal_operator_t,
      public virtual mitl2gta::mitl::unary_operator_t {
public:
  timed_finally_t(
      mitl2gta::nonempty_interval_t const &interval,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  timed_finally_t(timed_finally_t const &) = default;
  timed_finally_t(timed_finally_t &&) = default;
  timed_finally_t &operator=(timed_finally_t const &) = delete;
  timed_finally_t &operator=(timed_finally_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class untimed_globally_t final
    : public virtual mitl2gta::mitl::unary_operator_t {
public:
  explicit untimed_globally_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  untimed_globally_t(untimed_globally_t const &) = default;
  untimed_globally_t(untimed_globally_t &&) = default;
  untimed_globally_t &operator=(untimed_globally_t const &) = delete;
  untimed_globally_t &operator=(untimed_globally_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class timed_globally_t final
    : public virtual mitl2gta::mitl::timed_temporal_operator_t,
      public virtual mitl2gta::mitl::unary_operator_t {
public:
  timed_globally_t(
      mitl2gta::nonempty_interval_t const &interval,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula);

  timed_globally_t(timed_globally_t const &) = default;
  timed_globally_t(timed_globally_t &&) = default;
  timed_globally_t &operator=(timed_globally_t const &) = delete;
  timed_globally_t &operator=(timed_globally_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class untimed_until_t final : public virtual mitl2gta::mitl::binary_operator_t {
public:
  untimed_until_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula);

  untimed_until_t(untimed_until_t const &) = default;
  untimed_until_t(untimed_until_t &&) = default;
  untimed_until_t &operator=(untimed_until_t const &) = delete;
  untimed_until_t &operator=(untimed_until_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class timed_until_t final
    : public virtual mitl2gta::mitl::timed_temporal_operator_t,
      public virtual mitl2gta::mitl::binary_operator_t {
public:
  timed_until_t(
      mitl2gta::nonempty_interval_t const &interval,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula);

  timed_until_t(timed_until_t const &) = default;
  timed_until_t(timed_until_t &&) = default;
  timed_until_t &operator=(timed_until_t const &) = delete;
  timed_until_t &operator=(timed_until_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class untimed_since_t final : public virtual mitl2gta::mitl::binary_operator_t {
public:
  untimed_since_t(
      std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula);

  untimed_since_t(untimed_since_t const &) = default;
  untimed_since_t(untimed_since_t &&) = default;
  untimed_since_t &operator=(untimed_since_t const &) = delete;
  untimed_since_t &operator=(untimed_since_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class timed_since_t final
    : public virtual mitl2gta::mitl::timed_temporal_operator_t,
      public virtual mitl2gta::mitl::binary_operator_t {
public:
  timed_since_t(
      mitl2gta::nonempty_interval_t const &interval,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
      std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula);

  timed_since_t(timed_since_t const &) = default;
  timed_since_t(timed_since_t &&) = default;
  timed_since_t &operator=(timed_since_t const &) = delete;
  timed_since_t &operator=(timed_since_t &&) = delete;

  std::ostream &output(std::ostream &os) const override;

  void visit(mitl2gta::mitl::formula_visitor_t &visitor) const override;
};

class formula_visitor_t {
public:
  virtual void visit(mitl2gta::mitl::atomic_proposition_t const &) = 0;
  virtual void visit(mitl2gta::mitl::atomic_constant_t const &) = 0;
  virtual void visit(mitl2gta::mitl::negation_t const &) = 0;
  virtual void visit(mitl2gta::mitl::or_t const &) = 0;
  virtual void visit(mitl2gta::mitl::and_t const &) = 0;
  virtual void visit(mitl2gta::mitl::untimed_next_t const &) = 0;
  virtual void visit(mitl2gta::mitl::timed_next_t const &) = 0;
  virtual void visit(mitl2gta::mitl::untimed_yesterday_t const &) = 0;
  virtual void visit(mitl2gta::mitl::timed_yesterday_t const &) = 0;
  virtual void visit(mitl2gta::mitl::untimed_finally_t const &) = 0;
  virtual void visit(mitl2gta::mitl::timed_finally_t const &) = 0;
  virtual void visit(mitl2gta::mitl::untimed_globally_t const &) = 0;
  virtual void visit(mitl2gta::mitl::timed_globally_t const &) = 0;
  virtual void visit(mitl2gta::mitl::untimed_until_t const &) = 0;
  virtual void visit(mitl2gta::mitl::timed_until_t const &) = 0;
  virtual void visit(mitl2gta::mitl::untimed_since_t const &) = 0;
  virtual void visit(mitl2gta::mitl::timed_since_t const &) = 0;
};

} // namespace mitl

} // namespace mitl2gta
