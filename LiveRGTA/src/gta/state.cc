/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/gta/state.hh"

namespace tchecker {

namespace gta {

state_t::state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval)
    : tchecker::ta::state_t(vloc, intval)
{
}

state_t::state_t(tchecker::gta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval)
    : tchecker::ta::state_t(s, vloc, intval)
{
}

bool operator==(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2)
{
  return tchecker::ta::operator==(s1, s2);
}

bool operator!=(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2) { return !(s1 == s2); }

bool shared_equal_to(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2)
{
  return (tchecker::ta::shared_equal_to(s1, s2));
}

std::size_t hash_value(tchecker::gta::state_t const & s) { return tchecker::ta::hash_value(s); }

std::size_t shared_hash_value(tchecker::gta::state_t const & s) { return tchecker::ta::shared_hash_value(s); }

int lexical_cmp(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2)
{
  return tchecker::ta::lexical_cmp(s1, s2);
}

} // end of namespace gta

} // end of namespace tchecker
