/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gta/gta.hh"

namespace tchecker {

namespace gta {

tchecker::state_status_t initial(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                 tchecker::intval_sptr_t const & intval, tchecker::vedge_sptr_t const & vedge,
                                 tchecker::sync_id_t & sync_id, tchecker::gta::initial_value_t const & initial_range)
{
  tchecker::clock_constraint_container_t placeholder_clock_constraints;
  return tchecker::ta::initial(system.as_ta_system(), vloc, intval, vedge, sync_id, placeholder_clock_constraints,
                               initial_range);
}

tchecker::state_status_t next(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::intval_sptr_t const & intval, tchecker::vedge_sptr_t const & vedge,
                              tchecker::sync_id_t & sync_id, tchecker::gta::outgoing_edges_value_t const & sync_edges)
{
  tchecker::clock_constraint_container_t placeholder_clock_constraints;
  tchecker::clock_reset_container_t placeholder_resets;
  return tchecker::ta::next(system.as_ta_system(), vloc, intval, vedge, sync_id, placeholder_clock_constraints,
                            placeholder_clock_constraints, placeholder_resets, placeholder_clock_constraints, sync_edges);
}

// gta_t
gta_t::gta_t(std::shared_ptr<tchecker::gta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
             std::size_t block_size, std::size_t table_size)
    : _system(system), _sharing_type(sharing_type),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), table_size),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
{
}

// Forward

tchecker::gta::initial_range_t gta_t::initial_edges() { return tchecker::gta::initial_edges(*_system); }

void gta_t::initial(initial_value_t const & init_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::gta::state_sptr_t s = _state_allocator.construct();
  tchecker::gta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::gta::initial(*_system, *s, *t, init_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void gta_t::initial(std::vector<sst_t> & v, tchecker::state_status_t mask) { tchecker::ts::initial(*this, v, mask); }

outgoing_edges_range_t gta_t::outgoing_edges(tchecker::gta::const_state_sptr_t const & s)
{
  return tchecker::gta::outgoing_edges(*_system, s->vloc_ptr());
}

void gta_t::next(tchecker::gta::const_state_sptr_t const & s, tchecker::gta::outgoing_edges_value_t const & out_edge,
                 std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::gta::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::gta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::gta::next(*_system, *nexts, *t, out_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(nexts);
      share(t);
    }
    v.push_back(std::make_tuple(status, nexts, t));
  }
}

void gta_t::next(tchecker::gta::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::next(*this, s, v, mask);
}

// Inspector

boost::dynamic_bitset<> gta_t::labels(tchecker::gta::const_state_sptr_t const & s) const
{
  return tchecker::gta::labels(*_system, *s);
}

void gta_t::attributes(tchecker::gta::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::gta::attributes(*_system, *s, m);
}

void gta_t::attributes(tchecker::gta::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::gta::attributes(*_system, *t, m);
}

bool gta_t::is_valid_final(tchecker::gta::const_state_sptr_t const & s) const
{
  return tchecker::gta::is_valid_final(*_system, *s);
}

bool gta_t::is_initial(tchecker::gta::const_state_sptr_t const & s) const { return tchecker::gta::is_initial(*_system, *s); }

// Sharing

void gta_t::share(tchecker::gta::state_sptr_t & s) { _state_allocator.share(s); }

void gta_t::share(tchecker::gta::transition_sptr_t & t) { _transition_allocator.share(t); }

} // end of namespace gta

} // end of namespace tchecker
