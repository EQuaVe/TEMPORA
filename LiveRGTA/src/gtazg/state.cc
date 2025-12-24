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

#include "tchecker/gtazg/state.hh"

namespace tchecker {

namespace gtazg {

state_t::state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                 tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t> const & zone)
    : tchecker::gta::state_t(vloc, intval), _zone(zone)
{
  assert(zone.ptr() != nullptr);
}

state_t::state_t(tchecker::gta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                 tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t> const & zone)
    : tchecker::gta::state_t(s, vloc, intval), _zone(zone)
{
  assert(_zone.ptr() != nullptr);
}

bool operator==(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2)
{
  return tchecker::gta::operator==(s1, s2) && (s1.zone() == s2.zone());
}

bool operator!=(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2) { return !(s1 == s2); }

bool shared_equal_to(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2)
{
  return tchecker::gta::shared_equal_to(s1, s2) && (s1.zone_ptr() == s2.zone_ptr());
}

bool is_gmap_le(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2,
                tchecker::clockbounds::gta::vloc_to_bounds_t const & vloc_to_bounds)
{
  if (tchecker::gta::operator!=(s1, s2)) {
    return false;
  }

  tchecker::clockbounds::gta::gsim_map_t & vloc_clockbounds = vloc_to_bounds.bounds_of_vloc(s1.vloc_ptr());

  return (s1.zone().is_gmap_le(s2.zone(), vloc_clockbounds));
}

bool shared_is_gmap_le(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2,
                       tchecker::clockbounds::gta::vloc_to_bounds_t const & vloc_to_bounds)
{
  if (not tchecker::gta::shared_equal_to(s1, s2)) {
    return false;
  }

  if (s1.zone_ptr() == s2.zone_ptr()) {
    return true;
  }

  tchecker::clockbounds::gta::gsim_map_t & vloc_clockbounds = vloc_to_bounds.bounds_of_vloc(s1.vloc_ptr());
  return (s1.zone().is_gmap_le(s2.zone(), vloc_clockbounds));
}

std::size_t hash_value(tchecker::gtazg::state_t const & s)
{
  std::size_t h = tchecker::gta::hash_value(s);
  boost::hash_combine(h, s.zone());
  return h;
}

std::size_t shared_hash_value(tchecker::gtazg::state_t const & s)
{
  std::size_t h = tchecker::gta::shared_hash_value(s);
  boost::hash_combine(h, s.zone_ptr());
  return h;
}

std::size_t shared_partial_hash_value(tchecker::gtazg::state_t const & s)
{
  std::size_t h1 = tchecker::gta::shared_hash_value(s);
  std::size_t const h2 = s.zone().prophecy_clks_lower_bound_hash();
  boost::hash_combine(h1, h2);
  return h1;
}

int lexical_cmp(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2)
{
  int gta_cmp = tchecker::gta::lexical_cmp(s1, s2);
  if (gta_cmp != 0)
    return gta_cmp;
  return s1.zone().lexical_cmp(s2.zone());
}

} // end of namespace gtazg

} // end of namespace tchecker
