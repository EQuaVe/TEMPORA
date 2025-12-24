/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>
#include <stdexcept>

#include "tchecker/basictypes.hh"
#include "tchecker/gta_program/guard.hh"

namespace tchecker {
namespace gta {

// guard_term_t

std::string guard_term_t::to_string() const
{
  std::stringstream s;
  this->output(s);
  return s.str();
}

std::ostream & operator<<(std::ostream & os, tchecker::gta::guard_lterm_t const & term) { return term.output(os); }

std::ostream & operator<<(std::ostream & os, enum tchecker::gta::guard_operator_t op)
{
  switch (op) {
  case GUARD_EQ:
    return os << "==";

  case GUARD_LE:
    return os << "<=";

  case GUARD_LT:
    return os << "<";

  case GUARD_GE:
    return os << ">=";

  case GUARD_GT:
    return os << ">";

  default:
    throw std::invalid_argument("Not a recognised guard operator");
  }
}

std::ostream & operator<<(std::ostream & os, enum tchecker::gta::guard_rterm_special_constants_t c)

{
  switch (c) {
  case RTERM_INF:
    return os << "inf";
  case RTERM_MINUS_INF:
    return os << "-inf";
  default:
    throw std::invalid_argument("Not a recognised rterm constant");
  }
}

// guard_variable_t
guard_variable_t::guard_variable_t(std::string const & name) : _name(name)
{
  if (_name.empty())
    throw std::invalid_argument("empty variable name");
}

std::ostream & guard_variable_t::output(std::ostream & os) const { return os << _name; }

tchecker::gta::guard_variable_t * guard_variable_t::clone() const { return (new tchecker::gta::guard_variable_t(_name)); }

void guard_variable_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// guard_integer_t
guard_integer_t::guard_integer_t(tchecker::integer_t const val) : _val(val) {}

std::ostream & guard_integer_t::output(std::ostream & os) const { return os << _val; }

tchecker::gta::guard_integer_t * guard_integer_t::clone() const { return (new tchecker::gta::guard_integer_t(_val)); }

void guard_integer_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// guard_unary_lterm_t
guard_unary_lterm_t::guard_unary_lterm_t(std::shared_ptr<tchecker::gta::guard_variable_t const> const & var) : _var(var)
{
  if (var.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for var");
  }
}

std::ostream & guard_unary_lterm_t::output(std::ostream & os) const
{
  _var->output(os);
  return os;
}

tchecker::gta::guard_unary_lterm_t * guard_unary_lterm_t::clone() const
{
  const std::shared_ptr<tchecker::gta::guard_variable_t const> variable_clone{_var->clone()};
  return new tchecker::gta::guard_unary_lterm_t(variable_clone);
}

void guard_unary_lterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// guard_binary_lterm_t
guard_binary_lterm_t::guard_binary_lterm_t(std::shared_ptr<tchecker::gta::guard_variable_t const> const & var_l,
                                           std::shared_ptr<tchecker::gta::guard_variable_t const> const & var_r)
    : _var_l(var_l), _var_r(var_r)
{
  if (var_l.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for var_l");
  }

  if (var_r.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for var_r");
  }
}

std::ostream & guard_binary_lterm_t::output(std::ostream & os) const
{
  _var_l->output(os);
  os << "-";
  _var_r->output(os);
  return os;
}
tchecker::gta::guard_binary_lterm_t * guard_binary_lterm_t::clone() const
{
  const std::shared_ptr<tchecker::gta::guard_variable_t const> left_variable_clone{_var_l->clone()};

  const std::shared_ptr<tchecker::gta::guard_variable_t const> right_variable_clone{_var_r->clone()};
  return new tchecker::gta::guard_binary_lterm_t(left_variable_clone, right_variable_clone);
}

void guard_binary_lterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// guard_integer_rterm_t
guard_integer_rterm_t::guard_integer_rterm_t(std::shared_ptr<tchecker::gta::guard_integer_t const> const & val) : _val(val)
{
  if (val.get() == nullptr) {
    throw std::invalid_argument("Value passed as nullptr");
  }
}

std::ostream & guard_integer_rterm_t::output(std::ostream & os) const
{
  _val->output(os);
  return os;
}

tchecker::gta::guard_integer_rterm_t * guard_integer_rterm_t::clone() const
{
  std::shared_ptr<tchecker::gta::guard_integer_t const> const val_clone{_val->clone()};
  return new tchecker::gta::guard_integer_rterm_t(val_clone);
}

void guard_integer_rterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// guard_constant_rterm_t
guard_constant_rterm_t::guard_constant_rterm_t(enum tchecker::gta::guard_rterm_special_constants_t c) : _c(c) {}

std::ostream & guard_constant_rterm_t::output(std::ostream & os) const { return os << _c; }

tchecker::gta::guard_constant_rterm_t * guard_constant_rterm_t::clone() const
{
  return new tchecker::gta::guard_constant_rterm_t(_c);
}

void guard_constant_rterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// guard_t
guard_t::guard_t(std::shared_ptr<tchecker::gta::guard_lterm_t const> const & lterm, enum tchecker::gta::guard_operator_t op,
                 std::shared_ptr<tchecker::gta::guard_rterm_t const> const & rterm)
    : _lterm(lterm), _op(op), _rterm(rterm)
{
  if (lterm.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for lterm");
  }

  if (rterm.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for rterm");
  }
}

std::ostream & guard_t::output(std::ostream & os) const
{
  _lterm->output(os);
  os << _op;
  _rterm->output(os);
  return os;
}

tchecker::gta::guard_t * guard_t::clone() const
{
  const std::shared_ptr<tchecker::gta::guard_lterm_t const> lterm_clone{_lterm->clone()};

  const std::shared_ptr<tchecker::gta::guard_rterm_t const> rterm_clone{_rterm->clone()};
  return new tchecker::gta::guard_t(lterm_clone, _op, rterm_clone);
}

void guard_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// guards_conjunction_t
std::ostream & operator<<(std::ostream & os, tchecker::gta::guards_conjunction_t & guards)
{
  for (auto guard_iterator = guards.begin(); guard_iterator != guards.end(); guard_iterator++) {
    if (guard_iterator != guards.begin()) {
      os << " && ";
    }
    (*guard_iterator)->output(os);
  }
  return os;
}
} // end of namespace gta
} // end of namespace tchecker
