/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gta_program/gta_program.hh"
#include <stdexcept>

namespace tchecker {
namespace gta {

// gta_program_t
std::ostream & gta_program_t::output(std::ostream & os) const
{

  for (tchecker::gta::gta_program_t::iterator_t curr = begin(); curr != end(); curr = next(curr)) {
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

std::string gta_program_t::to_string() const
{
  std::stringstream s;
  this->output(s);
  return s.str();
}

void gta_program_t::append_guards(tchecker::gta::guards_conjunction_ptr_t const & guards)
{
  if (guards.get() == nullptr) {
    throw std::invalid_argument("null pointer passed for guards");
  }
  _guards_actions_list.append_t1(guards);
}

void gta_program_t::append_actions(tchecker::gta::actions_ptr_t const & actions)
{
  if (actions.get() == nullptr) {
    throw std::invalid_argument("null pointer passed for actions");
  }
  _guards_actions_list.append_t2(actions);
}

guards_node_ptr_t const gta_program_t::beginning_guards() const { return _guards_actions_list.get_head(); }

std::pair<guards_node_ptr_t, actions_node_ptr_t> const gta_program_t::end_guards_or_actions() const
{
  return _guards_actions_list.get_tail();
}

tchecker::gta::gta_program_t::iterator_t gta_program_t::begin() const { return _guards_actions_list.begin(); }

tchecker::gta::gta_program_t::iterator_t gta_program_t::end() const { return _guards_actions_list.end(); }

tchecker::gta::gta_program_t::iterator_t gta_program_t::next(tchecker::gta::gta_program_t::iterator_t & curr) const
{
  return _guards_actions_list.next(curr);
}

std::ostream & operator<<(std::ostream & os, tchecker::gta::gta_program_t const & gta_program)
{
  return gta_program.output(os);
}
} // end of namespace gta
} // end of namespace tchecker
