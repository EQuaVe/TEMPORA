/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>
#include <stdexcept>

#include "tchecker/gta_program/action.hh"

namespace tchecker {
namespace gta {

// action_t
std::string action_t::to_string() const
{
  std::stringstream s;
  this->output(s);
  return s.str();
}

std::ostream & operator<<(std::ostream & os, tchecker::gta::action_t const & action) { return action.output(os); }

// action_variable_t
action_variable_t::action_variable_t(std::string const & name) : _name(name)
{
  if (name.empty()) {
    throw std::invalid_argument("Empty variable name not allowed");
  }
}

tchecker::gta::action_variable_t * action_variable_t::clone() const { return new action_variable_t(_name); }

void action_variable_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

std::ostream & action_variable_t::output(std::ostream & os) const { return os << _name; }

// release_reset_action_t
release_reset_action_t::release_reset_action_t(std::shared_ptr<tchecker::gta::action_variable_t> const & clk_var)
    : _clk_var(clk_var)
{
  if (clk_var.get() == nullptr) {
    throw std::invalid_argument("Null clock variable passed");
  }
}

tchecker::gta::release_reset_action_t * release_reset_action_t::clone() const
{
  std::shared_ptr<tchecker::gta::action_variable_t> clk_var_clone{_clk_var->clone()};
  return new release_reset_action_t(clk_var_clone);
}

void release_reset_action_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

std::ostream & release_reset_action_t::output(std::ostream & os) const
{
  os << "[";
  _clk_var->output(os);
  os << "]";
  return os;
}

// rename_action_t
rename_action_t::rename_action_t(std::shared_ptr<tchecker::gta::action_variable_t> const & lhs_clk_var,
                                 std::shared_ptr<tchecker::gta::action_variable_t> const & rhs_clk_var)
    : _lhs_clk_var(lhs_clk_var), _rhs_clk_var(rhs_clk_var)
{
  if (lhs_clk_var.get() == nullptr) {
    throw std::invalid_argument("LHS clock variable null");
  }

  if (rhs_clk_var.get() == nullptr) {
    throw std::invalid_argument("RHS clock variable null");
  }
}

tchecker::gta::rename_action_t * rename_action_t::clone() const
{
  std::shared_ptr<tchecker::gta::action_variable_t> lhs_clk_var_clone{_lhs_clk_var->clone()};
  std::shared_ptr<tchecker::gta::action_variable_t> rhs_clk_var_clone{_rhs_clk_var->clone()};
  return new rename_action_t(lhs_clk_var_clone, rhs_clk_var_clone);
}

void rename_action_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

std::ostream & rename_action_t::output(std::ostream & os) const
{
  _lhs_clk_var->output(os);
  os << "=";
  _rhs_clk_var->output(os);
  return os;
}

// actions_t
std::ostream & operator<<(std::ostream & os, tchecker::gta::actions_t & actions_list)
{
  for (auto action_iterator = actions_list.begin(); action_iterator != actions_list.end(); action_iterator++) {
    if (action_iterator != actions_list.begin()) {
      os << ",";
    }
    (*action_iterator)->output(os);
  }
  return os;
}
} // end of namespace gta
} // end of namespace tchecker
