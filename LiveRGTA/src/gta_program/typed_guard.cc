/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iostream>
#include <stdexcept>

#include "tchecker/gta_program/typed_guard.hh"

namespace tchecker {

namespace gta {

// enum guard_term_type_t
std::ostream & operator<<(std::ostream & os, enum tchecker::gta::guard_term_type_t const type)
{
  switch (type) {
  case GUARD_TYPE_BAD:
    return os << "BAD";
  case GUARD_TYPE_CLK_VAR:
    return os << "CLK_VAR";
  case GUARD_TYPE_INTEGER:
    return os << "INTEGER";
  case GUARD_TYPE_UNARY_LTERM:
    return os << "UNARY_LTERM";
  case GUARD_TYPE_BINARY_LTERM:
    return os << "BINARY_LTERM";
  case GUARD_TYPE_INTEGER_RTERM:
    return os << "INTEGER_RTERM";
  case GUARD_TYPE_CONSTANT_RTERM:
    return os << "CONSTANT_RTERM";
  case GUARD_TYPE_SIMPLE:
    return os << "SIMPLE_GUARD";
  case GUARD_TYPE_DIAGONAL:
    return os << "DIAGONAL_GUARD";
  default:
    throw std::invalid_argument("Unrecognised guard type");
  }
}

enum tchecker::gta::guard_term_type_t guard_type_from_lterm_type(enum tchecker::gta::guard_term_type_t const lterm_type)
{
  switch (lterm_type) {
  case GUARD_TYPE_UNARY_LTERM:
    return GUARD_TYPE_SIMPLE;

  case GUARD_TYPE_BINARY_LTERM:
    return GUARD_TYPE_DIAGONAL;

  default:
    throw std::invalid_argument("Not a valid type for a lterm");
  }
}

// typed_guard_term_t

typed_guard_term_t::typed_guard_term_t(enum tchecker::gta::guard_term_type_t const type) : _type(type) {}

// typed_guard_clk_variable_t

typed_guard_clk_variable_t::typed_guard_clk_variable_t(std::string const & name, tchecker::clock_id_t const clk_id,
                                                       enum tchecker::gta::clk_type_t const clk_type)
    : tchecker::gta::typed_guard_term_t(GUARD_TYPE_CLK_VAR), tchecker::gta::guard_variable_t(name), _clk_id(clk_id),
      _clk_type(clk_type)
{
}

tchecker::gta::typed_guard_clk_variable_t * typed_guard_clk_variable_t::clone() const
{
  return new typed_guard_clk_variable_t(_name, _clk_id, _clk_type);
}

void typed_guard_clk_variable_t::visit(tchecker::gta::typed_guard_visitor_t & v) const { v.visit(*this); }

void typed_guard_clk_variable_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// typed_guard_integer_t

typed_guard_integer_t::typed_guard_integer_t(tchecker::integer_t val)
    : tchecker::gta::typed_guard_term_t(GUARD_TYPE_INTEGER), tchecker::gta::guard_integer_t(val)
{
}

tchecker::gta::typed_guard_integer_t * typed_guard_integer_t::clone() const { return new typed_guard_integer_t(_val); }

void typed_guard_integer_t::visit(tchecker::gta::typed_guard_visitor_t & v) const { v.visit(*this); }

void typed_guard_integer_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// typed_guard_unary_lterm_t

typed_guard_unary_lterm_t::typed_guard_unary_lterm_t(std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const var)
    : tchecker::gta::typed_guard_term_t(GUARD_TYPE_UNARY_LTERM), tchecker::gta::guard_unary_lterm_t(var), _typed_var(var)
{
}

tchecker::gta::typed_guard_unary_lterm_t * typed_guard_unary_lterm_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> typed_var_clone{_typed_var->clone()};
  return new typed_guard_unary_lterm_t(typed_var_clone);
}

void typed_guard_unary_lterm_t::visit(tchecker::gta::typed_guard_visitor_t & v) const { v.visit(*this); }

void typed_guard_unary_lterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// typed_guard_binary_lterm_t

typed_guard_binary_lterm_t::typed_guard_binary_lterm_t(std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const var_l,
                                                       std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const var_r)
    : tchecker::gta::typed_guard_term_t(GUARD_TYPE_BINARY_LTERM), tchecker::gta::guard_binary_lterm_t(var_l, var_r),
      _typed_var_l(var_l), _typed_var_r(var_r)
{
}

tchecker::gta::typed_guard_binary_lterm_t * typed_guard_binary_lterm_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> typed_var_l_clone{_typed_var_l->clone()};
  std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> typed_var_r_clone{_typed_var_r->clone()};
  return new typed_guard_binary_lterm_t(typed_var_l_clone, typed_var_r_clone);
}

void typed_guard_binary_lterm_t::visit(tchecker::gta::typed_guard_visitor_t & v) const { v.visit(*this); }

void typed_guard_binary_lterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// typed_guard_integer_rterm_t

typed_guard_integer_rterm_t::typed_guard_integer_rterm_t(std::shared_ptr<tchecker::gta::typed_guard_integer_t> const val)
    : tchecker::gta::typed_guard_term_t(GUARD_TYPE_INTEGER_RTERM), tchecker::gta::guard_integer_rterm_t(val), _typed_val(val)
{
}

tchecker::gta::typed_guard_integer_rterm_t * typed_guard_integer_rterm_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_guard_integer_t> typed_val_clone{_typed_val->clone()};
  return new typed_guard_integer_rterm_t(typed_val_clone);
}

void typed_guard_integer_rterm_t::visit(tchecker::gta::typed_guard_visitor_t & v) const { v.visit(*this); }

void typed_guard_integer_rterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// typed_guard_constant_rterm_t

typed_guard_constant_rterm_t::typed_guard_constant_rterm_t(enum tchecker::gta::guard_rterm_special_constants_t const c)
    : tchecker::gta::typed_guard_term_t(GUARD_TYPE_CONSTANT_RTERM), tchecker::gta::guard_constant_rterm_t(c)
{
}

tchecker::gta::typed_guard_constant_rterm_t * typed_guard_constant_rterm_t::clone() const
{
  return new typed_guard_constant_rterm_t(_c);
}

void typed_guard_constant_rterm_t::visit(tchecker::gta::typed_guard_visitor_t & v) const { v.visit(*this); }

void typed_guard_constant_rterm_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// typed_guard_t

typed_guard_t::typed_guard_t(std::shared_ptr<tchecker::gta::typed_guard_lterm_t> const lterm,
                             enum tchecker::gta::guard_operator_t const op,
                             std::shared_ptr<tchecker::gta::typed_guard_rterm_t> const rterm)
    : tchecker::gta::typed_guard_term_t(tchecker::gta::guard_type_from_lterm_type(lterm->type())),
      tchecker::gta::guard_t(lterm, op, rterm), _typed_lterm(lterm), _typed_rterm(rterm)
{
  if (_type == tchecker::gta::guard_term_type_t::GUARD_TYPE_DIAGONAL) {
    if (_op == tchecker::gta::guard_operator_t::GUARD_EQ || _op == tchecker::gta::guard_operator_t::GUARD_GE ||
        _op == tchecker::gta::guard_operator_t::GUARD_GT) {
      throw std::invalid_argument(
          "Diagonal constraints must only have less than or less equals operator. See documentation for more details");
    }
  }
}

tchecker::gta::typed_guard_t * typed_guard_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_guard_lterm_t> lterm_clone{_typed_lterm->clone()};
  std::shared_ptr<tchecker::gta::typed_guard_rterm_t> rterm_clone{_typed_rterm->clone()};

  return new typed_guard_t(lterm_clone, _op, rterm_clone);
}

void typed_guard_t::visit(tchecker::gta::typed_guard_visitor_t & v) const { v.visit(*this); }

void typed_guard_t::visit(tchecker::gta::guard_visitor_t & v) const { v.visit(*this); }

// typed_guards_conjunction_t
std::ostream & operator<<(std::ostream & os, tchecker::gta::typed_guards_conjunction_t & typed_guards)
{
  for (auto guard_iterator = typed_guards.begin(); guard_iterator != typed_guards.end(); guard_iterator++) {
    if (guard_iterator != typed_guards.begin()) {
      os << " && ";
    }
    (*guard_iterator)->output(os);
  }
  return os;
}
} // end of namespace gta

} // end of namespace tchecker
