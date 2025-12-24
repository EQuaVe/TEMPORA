/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iostream>
#include <stdexcept>

#include "tchecker/gta_program/typed_action.hh"

namespace tchecker {

namespace gta {

// enum action_type_t

std::ostream & operator<<(std::ostream & os, enum tchecker::gta::action_type_t type)
{
  switch (type) {
  case ACTION_TYPE_BAD:
    return os << "BAD";
  case ACTION_TYPE_CLK_VAR:
    return os << "CLK_VAR";
  case ACTION_TYPE_RELEASE_PROPHECY:
    return os << "RELEASE";
  case ACTION_TYPE_RESET_HISTORY:
    return os << "RESET";
  case ACTION_TYPE_RENAME_PROPHECY:
    return os << "RENAME_PROPHECY";
  case ACTION_TYPE_RENAME_HISTORY:
    return os << "RENAME_HISTORY";
  default:
    throw std::invalid_argument("Unrecognised action type");
  }
}

/* typed_statement_t */

typed_action_t::typed_action_t(enum tchecker::gta::action_type_t type) : _type(type) {}

/* typed_action_clk_variable_t */

typed_action_clk_variable_t::typed_action_clk_variable_t(std::string const & name, tchecker::clock_id_t const clk_id,
                                                         enum tchecker::gta::clk_type_t const clk_type)
    : tchecker::gta::typed_action_t(ACTION_TYPE_CLK_VAR), tchecker::gta::action_variable_t(name), _clk_id(clk_id),
      _clk_type(clk_type)
{
}

tchecker::gta::typed_action_clk_variable_t * typed_action_clk_variable_t::clone() const
{
  return new tchecker::gta::typed_action_clk_variable_t(_name, _clk_id, _clk_type);
}

void typed_action_clk_variable_t::visit(tchecker::gta::typed_action_visitor_t & v) const { v.visit(*this); }

void typed_action_clk_variable_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

/* typed_action_release_t */

typed_action_release_t::typed_action_release_t(std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & clk_var)
    : tchecker::gta::typed_action_t(ACTION_TYPE_RELEASE_PROPHECY), tchecker::gta::release_reset_action_t(clk_var),
      _typed_clk_var(clk_var)
{
  if (clk_var->clk_type() != tchecker::gta::GTA_CLK_TYPE_PROPHECY) {
    throw std::invalid_argument("Release action is defined only for prophecy clocks");
  }
}

tchecker::gta::typed_action_release_t * typed_action_release_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> typed_clk_var_clone{_typed_clk_var->clone()};
  return new tchecker::gta::typed_action_release_t(typed_clk_var_clone);
}

void typed_action_release_t::visit(tchecker::gta::typed_action_visitor_t & v) const { v.visit(*this); }

void typed_action_release_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

/* typed_action_reset_t */

typed_action_reset_t::typed_action_reset_t(std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & clk_var)
    : tchecker::gta::typed_action_t(ACTION_TYPE_RESET_HISTORY), tchecker::gta::release_reset_action_t(clk_var),
      _typed_clk_var(clk_var)
{
  if (clk_var->clk_type() != tchecker::gta::GTA_CLK_TYPE_HISTORY)
    throw std::invalid_argument("Reset action is defined only for history clocks");
}

tchecker::gta::typed_action_reset_t * typed_action_reset_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> typed_clk_var_clone{_typed_clk_var->clone()};
  return new tchecker::gta::typed_action_reset_t(typed_clk_var_clone);
}

void typed_action_reset_t::visit(tchecker::gta::typed_action_visitor_t & v) const { v.visit(*this); }

void typed_action_reset_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

/* typed_action_rename_prophecy_t */

typed_action_rename_prophecy_t::typed_action_rename_prophecy_t(

    std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & lhs_clk_var,
    std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & rhs_clk_var)
    : tchecker::gta::typed_action_t(ACTION_TYPE_RENAME_PROPHECY), tchecker::gta::rename_action_t(lhs_clk_var, rhs_clk_var),
      _typed_lhs_clk_var(lhs_clk_var), _typed_rhs_clk_var(rhs_clk_var)
{
  if (lhs_clk_var->clk_type() != tchecker::gta::GTA_CLK_TYPE_PROPHECY) {
    throw std::invalid_argument("LHS clock must be prophecy");
  }

  if (rhs_clk_var->clk_type() != tchecker::gta::GTA_CLK_TYPE_PROPHECY) {
    throw std::invalid_argument("RHS clock must be prophecy");
  }
}

tchecker::gta::typed_action_rename_prophecy_t * typed_action_rename_prophecy_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> typed_lhs_clk_var_clone{_typed_lhs_clk_var->clone()};
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> typed_rhs_clk_var_clone{_typed_rhs_clk_var->clone()};
  return new tchecker::gta::typed_action_rename_prophecy_t(typed_lhs_clk_var_clone, typed_rhs_clk_var_clone);
}

void typed_action_rename_prophecy_t::visit(tchecker::gta::typed_action_visitor_t & v) const { v.visit(*this); }

void typed_action_rename_prophecy_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

/* typed_action_rename_history_t */

typed_action_rename_history_t::typed_action_rename_history_t(

    std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & lhs_clk_var,
    std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & rhs_clk_var)
    : tchecker::gta::typed_action_t(ACTION_TYPE_RENAME_HISTORY), tchecker::gta::rename_action_t(lhs_clk_var, rhs_clk_var),
      _typed_lhs_clk_var(lhs_clk_var), _typed_rhs_clk_var(rhs_clk_var)
{
  if (lhs_clk_var->clk_type() != tchecker::gta::GTA_CLK_TYPE_HISTORY) {
    throw std::invalid_argument("LHS clock must be history");
  }

  if (rhs_clk_var->clk_type() != tchecker::gta::GTA_CLK_TYPE_HISTORY) {
    throw std::invalid_argument("RHS clock must be history");
  }
}

tchecker::gta::typed_action_rename_history_t * typed_action_rename_history_t::clone() const
{
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> typed_lhs_clk_var_clone{_typed_lhs_clk_var->clone()};
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> typed_rhs_clk_var_clone{_typed_rhs_clk_var->clone()};
  return new tchecker::gta::typed_action_rename_history_t(typed_lhs_clk_var_clone, typed_rhs_clk_var_clone);
}

void typed_action_rename_history_t::visit(tchecker::gta::typed_action_visitor_t & v) const { v.visit(*this); }

void typed_action_rename_history_t::visit(tchecker::gta::action_visitor_t & v) const { v.visit(*this); }

std::ostream & operator<<(std::ostream & os, tchecker::gta::typed_actions_t & typed_actions_list)
{
  for (auto action_iterator = typed_actions_list.begin(); action_iterator != typed_actions_list.end(); action_iterator++) {
    if (action_iterator != typed_actions_list.begin()) {
      os << ",";
    }
    (*action_iterator)->output(os);
  }
  return os;
}

} // end of namespace gta

} // end of namespace tchecker
