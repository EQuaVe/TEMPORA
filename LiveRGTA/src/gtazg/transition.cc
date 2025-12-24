/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gta/transition.hh"
#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/gtazg/transition.hh"

namespace tchecker {

namespace gtazg {

transition_t::transition_t(tchecker::sync_id_t sync_id, tchecker::vedge_sptr_t const & vedge)
    : tchecker::gta::transition_t(sync_id, vedge)
{
}

transition_t::transition_t(tchecker::gtazg::transition_t const & t, tchecker::sync_id_t sync_id,
                           tchecker::vedge_sptr_t const & vedge)
    : tchecker::gta::transition_t(sync_id, vedge)
{
}

bool operator==(tchecker::gtazg::transition_t const & t1, tchecker::gtazg::transition_t const & t2)
{
  return tchecker::gta::operator==(t1, t2);
}

bool shared_equal_to(tchecker::gtazg::transition_t const & t1, tchecker::gtazg::transition_t const & t2)
{
  return tchecker::gta::shared_equal_to(t1, t2);
}

std::size_t hash_value(tchecker::gtazg::transition_t const & t) { return tchecker::gta::hash_value(t); }

std::size_t shared_hash_value(tchecker::gtazg::transition_t const & t) { return tchecker::gta::shared_hash_value(t); }

int lexical_cmp(tchecker::gtazg::transition_t const & t1, tchecker::gtazg::transition_t const & t2)
{

  return tchecker::gta::lexical_cmp(t1, t2);
}

} // end of namespace gtazg

} // end of namespace tchecker
