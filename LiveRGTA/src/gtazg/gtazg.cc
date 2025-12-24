/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gtazg/gtazg.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/gta/static_analysis.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/clocks.hh"

namespace tchecker {

namespace gtazg {

/* Semantics functions */

tchecker::state_status_t initial(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                 tchecker::intval_sptr_t const & intval, tchecker::gtazg::gta_zone_sptr_t const & zone,
                                 tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                                 tchecker::gtazg::standard_semantics_t & semantics,
                                 tchecker::gtazg::initial_value_t const & initial_range)
{
  tchecker::state_status_t status = tchecker::gta::initial(system, vloc, intval, vedge, sync_id, initial_range);
  if (status != tchecker::STATE_OK) {
    return status;
  }

  tchecker::dbm::db_t * dbm = zone->dbm();
  tchecker::clock_id_t dim = zone->dim();
  bool delay_allowed = tchecker::gta::delay_allowed(system, *vloc);

  tchecker::gtadbm::history_clks_idx_t history_zero_clks_idx(system.history_zero_clks());
  tchecker::gtadbm::history_clks_idx_t history_inf_clks_idx(system.history_inf_clks());

  status = semantics.initial(dbm, dim, delay_allowed, zone->prophecy_clks_idx(), history_zero_clks_idx, history_inf_clks_idx);
  if (status != tchecker::STATE_OK) {
    return status;
  }

  return tchecker::STATE_OK;
}

tchecker::state_status_t next(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::intval_sptr_t const & intval, tchecker::gtazg::gta_zone_sptr_t const & zone,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::gtazg::standard_semantics_t & semantics,
                              tchecker::gtazg::outgoing_edges_value_t const & sync_edges)
{
  bool src_delay_allowed = tchecker::gta::delay_allowed(system, *vloc);

  tchecker::state_status_t status = tchecker::gta::next(system, vloc, intval, vedge, sync_id, sync_edges);
  if (status != tchecker::STATE_OK) {
    return status;
  }

  tchecker::dbm::db_t * dbm = zone->dbm();
  tchecker::clock_id_t dim = zone->dim();
  tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx = zone->prophecy_clks_idx();
  tchecker::gtadbm::history_clks_idx_t const & history_clks_idx = zone->history_clks_idx();

  semantics.delay_before_transition(dbm, dim, src_delay_allowed, prophecy_clks_idx, history_clks_idx);

  for (auto const & e : sync_edges.edges) {
    tchecker::edge_id_t e_id = e->id();

    tchecker::gta::compiled_gta_program_t const & compiled_gta_prog = system.compiled_gta_program(e_id);

    status = semantics.execute_gta_program(dbm, dim, prophecy_clks_idx, history_clks_idx, compiled_gta_prog);
    if (status != tchecker::STATE_OK) {
      return status;
    }
  }

  semantics.delay_after_transition(dbm, dim, src_delay_allowed, prophecy_clks_idx, history_clks_idx);

  return tchecker::STATE_OK;
}

/* labels */

boost::dynamic_bitset<> labels(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s)
{
  return tchecker::gta::labels(system, s);
}

/* is_valid_final */
bool is_valid_final(tchecker::gta::system_t const & system, tchecker::gtazg::gta_zone_t const & zone)
{
  return zone.is_final();
}

bool is_valid_final(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s)
{
  return tchecker::gta::is_valid_final(system, s) && is_valid_final(system, s.zone());
}

/* is_initial */

bool is_initial(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s)
{
  return tchecker::gta::is_initial(system, s);
}

/* attributes */

void attributes(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s,
                std::map<std::string, std::string> & m)
{
  tchecker::gta::attributes(system, s, m);
  m["zone"] = tchecker::to_string(s.zone(), system.clock_variables().flattened().index());
}

void attributes(tchecker::ta::system_t const & system, tchecker::gtazg::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  tchecker::gta::attributes(system, t, m);
}

boost::dynamic_bitset<> released_clks(tchecker::gta::system_t const & system, tchecker::vedge_t const & t)
{
  tchecker::clock_id_t const num_clks = system.clocks_count(tchecker::VK_FLATTENED);
  boost::dynamic_bitset<> released_clks(num_clks);
  for (tchecker::edge_id_t id : t) {
    released_clks |= tchecker::gta::released_clks(system.gta_program(id), num_clks);
  }
  return released_clks;
}

boost::dynamic_bitset<> minus_inf_clks(tchecker::gta::system_t const & system, tchecker::gtazg::gta_zone_t const & zone)
{
  tchecker::clock_id_t const num_clks = system.clocks_count(tchecker::VK_FLATTENED);
  boost::dynamic_bitset<> minus_inf_clks(num_clks);

  for (clock_id_t x = 0; x < num_clks; x++) {
    if (system.is_prophecy_clock(x)) {
      tchecker::gtadbm::clk_idx_t const x_idx = x + 1;
      minus_inf_clks[x] = tchecker::gtadbm::can_be_minus_infinity(zone.dbm(), zone.dim(), x_idx, zone.prophecy_clks_idx(),
                                                                  zone.history_clks_idx());
    }
  }
  return minus_inf_clks;
}

/* gtazg_t */

gtazg_t::gtazg_t(std::shared_ptr<tchecker::gta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
                 std::shared_ptr<tchecker::gtazg::standard_semantics_t> const & semantics, std::size_t block_size,
                 std::size_t table_size)
    : _system(system), _prophecy_clks_idx(system->prophecy_clks()),
      _history_clks_idx(system->history_zero_clks() | system->history_inf_clks()), _sharing_type(sharing_type),
      _semantics(semantics),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), block_size,
                       _system->clocks_count(tchecker::VK_FLATTENED) + 1, table_size, _prophecy_clks_idx, _history_clks_idx),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
{
  if (!tchecker::gta::is_xd_safe(*_system)) {
    std::cerr << tchecker::log_warning << "GTA system is not X_D safe. Zone graph computation may not be finite." << std::endl;
  }
}

initial_range_t gtazg_t::initial_edges() { return tchecker::gtazg::initial_edges(*_system); }

void gtazg_t::initial(tchecker::gtazg::initial_value_t const & init_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::gtazg::state_sptr_t s = _state_allocator.construct();
  tchecker::gtazg::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::gtazg::initial(*_system, *s, *t, *_semantics, init_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void gtazg_t::initial(std::vector<sst_t> & v, tchecker::state_status_t mask) { tchecker::ts::initial(*this, v, mask); }

tchecker::gtazg::outgoing_edges_range_t gtazg_t::outgoing_edges(tchecker::gtazg::const_state_sptr_t const & s)
{
  return tchecker::gtazg::outgoing_edges(*_system, s->vloc_ptr());
}

void gtazg_t::next(tchecker::gtazg::const_state_sptr_t const & s, tchecker::gtazg::outgoing_edges_value_t const & out_edge,
                   std::vector<sst_t> & v, tchecker::state_status_t mask)
{

  for (auto const & e : out_edge.edges) {
    if (_system->is_edge_unsatisfiable(e->id())) {
      return;
    }
  }

  tchecker::gtazg::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::gtazg::transition_sptr_t nextt = _transition_allocator.construct();

  tchecker::state_status_t status = tchecker::gtazg::next(*_system, *nexts, *nextt, *_semantics, out_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(nexts);
      share(nextt);
    }
    v.push_back(std::make_tuple(status, nexts, nextt));
  }
}

void gtazg_t::next(tchecker::gtazg::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::next(*this, s, v, mask);
}

// Inspector

boost::dynamic_bitset<> gtazg_t::labels(tchecker::gtazg::const_state_sptr_t const & s) const
{
  return tchecker::gtazg::labels(*_system, *s);
}

void gtazg_t::attributes(tchecker::gtazg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::gtazg::attributes(*_system, *s, m);
}

void gtazg_t::attributes(tchecker::gtazg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::gtazg::attributes(*_system, *t, m);
}

void attributes(tchecker::gta::system_t const & system, tchecker::gtazg::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  tchecker::gta::attributes(system, t, m);
}

bool gtazg_t::is_valid_final(tchecker::gtazg::const_state_sptr_t const & s) const
{
  return tchecker::gtazg::is_valid_final(*_system, *s);
}

bool gtazg_t::is_initial(tchecker::gtazg::const_state_sptr_t const & s) const
{
  return tchecker::gtazg::is_initial(*_system, *s);
}

boost::dynamic_bitset<> gtazg_t::released_clks(tchecker::vedge_t const & t) const
{
  return tchecker::gtazg::released_clks(*_system, t);
}

boost::dynamic_bitset<> gtazg_t::minus_inf_clks(tchecker::gtazg::const_state_sptr_t const & s) const
{
  return tchecker::gtazg::minus_inf_clks(*_system, s->zone());
}

// Sharing

void gtazg_t::share(tchecker::gtazg::state_sptr_t & s) { _state_allocator.share(s); }

void gtazg_t::share(tchecker::gtazg::transition_sptr_t & t) { _transition_allocator.share(t); }

// Private

/* factory */

tchecker::gtazg::gtazg_t * factory(std::shared_ptr<tchecker::gta::system_t const> const & system,
                                   enum tchecker::ts::sharing_type_t sharing_type, std::size_t block_size,
                                   std::size_t table_size)
{
  std::shared_ptr<tchecker::gtazg::standard_semantics_t> semantics{tchecker::gtazg::semantics_factory()};
  return new tchecker::gtazg::gtazg_t(system, sharing_type, semantics, block_size, table_size);
}

} // namespace gtazg

} // end of namespace tchecker
