/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gta_program/typed_gta_program.hh"

namespace tchecker {
namespace gta {

// typed_gta_program_t
std::ostream & typed_gta_program_t::output(std::ostream & os) const
{

  for (tchecker::gta::typed_gta_program_t::iterator_t curr = begin(); curr != end(); curr = next(curr)) {
    if (curr.first != nullptr) {
      os << *(curr.first->get_value());
    }

    else if (curr.second != nullptr) {
      os << *(curr.second->get_value());
    }

    os << "; ";
  }
  return os;
}

std::string typed_gta_program_t::to_string() const
{
  std::stringstream s;
  this->output(s);
  return s.str();
}

void typed_gta_program_t::append_typed_guards(tchecker::gta::typed_guards_conjunction_ptr_t const & guards)
{
  if (guards.get() == nullptr) {
    throw std::invalid_argument("null pointer passed for guards");
  }
  _typed_guards_actions_list.append_t1(guards);
}

void typed_gta_program_t::append_typed_actions(tchecker::gta::typed_actions_ptr_t const & actions)
{
  if (actions.get() == nullptr) {
    throw std::invalid_argument("null pointer passed for actions");
  }
  _typed_guards_actions_list.append_t2(actions);
}

typed_guards_node_ptr_t const typed_gta_program_t::beginning_guards() const { return _typed_guards_actions_list.get_head(); }

std::pair<typed_guards_node_ptr_t, typed_actions_node_ptr_t> const typed_gta_program_t::end_guards_or_actions() const
{
  return _typed_guards_actions_list.get_tail();
}

tchecker::gta::typed_gta_program_t::iterator_t typed_gta_program_t::begin() const { return _typed_guards_actions_list.begin(); }

tchecker::gta::typed_gta_program_t::iterator_t typed_gta_program_t::end() const { return _typed_guards_actions_list.end(); }

tchecker::gta::typed_gta_program_t::iterator_t
typed_gta_program_t::next(tchecker::gta::typed_gta_program_t::iterator_t & curr) const
{
  return _typed_guards_actions_list.next(curr);
}

std::ostream & operator<<(std::ostream & os, tchecker::gta::typed_gta_program_t const & typed_gta_program)
{
  return typed_gta_program.output(os);
}
} // end of namespace gta
} // end of namespace tchecker
