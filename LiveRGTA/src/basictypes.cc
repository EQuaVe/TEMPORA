/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iostream>
#include <limits>
#include <sstream>

#include "tchecker/basictypes.hh"

namespace tchecker {

/* loc_id_t */

tchecker::loc_id_t const NO_LOC = std::numeric_limits<tchecker::loc_id_t>::max();

bool valid_loc_id(tchecker::loc_id_t id) { return (id != tchecker::NO_LOC); }

/* edge_id_t */

tchecker::edge_id_t const NO_EDGE = std::numeric_limits<tchecker::edge_id_t>::max();

bool valid_edge_id(tchecker::edge_id_t id) { return (id != tchecker::NO_EDGE); }

/* sync_id_t  */

tchecker::edge_id_t const NO_SYNC = std::numeric_limits<tchecker::sync_id_t>::max();

bool valid_sync_id(tchecker::sync_id_t id) { return id != tchecker::NO_SYNC; }

/* sync_strength_t */

std::ostream & operator<<(std::ostream & os, enum tchecker::sync_strength_t s)
{
  if (s == tchecker::SYNC_WEAK)
    os << "?";
  return os;
}

/* clock_rational_value_t */

std::ostream & operator<<(std::ostream & os, tchecker::clock_rational_value_t v)
{
  if (v.numerator() == 0 || v.denominator() == 1)
    os << v.numerator();
  else
    os << v.numerator() << "/" << v.denominator();
  return os;
}

std::string to_string(tchecker::clock_rational_value_t v)
{
  std::stringstream sstream;
  sstream << v;
  return sstream.str();
}

/* inequality comparator */

std::ostream & operator<<(std::ostream & os, tchecker::ineq_cmp_t cmp) { return os << (cmp == tchecker::LT ? "<" : "<="); }

std::string to_string(tchecker::ineq_cmp_t cmp)
{
  std::stringstream sstream;
  sstream << cmp;
  return sstream.str();
}

namespace gta {

std::ostream & operator<<(std::ostream & os, enum tchecker::gta::clk_type_t c)
{
  switch (c) {
  case GTA_CLK_TYPE_PROPHECY:
    return os << "prophecy";
  case GTA_CLK_TYPE_HISTORY:
    return os << "history";
  default:
    throw std::invalid_argument("Clock type not recognised");
  }
}
} // end of namespace gta

} // end of namespace tchecker
