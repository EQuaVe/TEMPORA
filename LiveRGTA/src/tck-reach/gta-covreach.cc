/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset.hpp>

#include "gta-covreach.hh"
#include "tchecker/algorithms/search_order.hh"
#include "tchecker/clockbounds/solver.hh"
#include "tchecker/gta/state.hh"
#include "tchecker/system/static_analysis.hh"
#include "tchecker/utils/log.hh"

namespace tchecker {

namespace tck_reach {

namespace gtacovreach {

/* node_t */

node_t::node_t(tchecker::gtazg::state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_gtazg_state_t(s)
{
}

node_t::node_t(tchecker::gtazg::const_state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_gtazg_state_t(s)
{
}

/* node_hash_t */

std::size_t node_hash_t::operator()(tchecker::tck_reach::gtacovreach::node_t const & n) const
{
  // NB: we hash on the discrete part of the state in n to check all nodes
  // with same discrete part for covering
  return tchecker::gta::shared_hash_value(n.state());
}

/* node_le_t */

node_le_t::node_le_t(std::shared_ptr<tchecker::clockbounds::gta::loc_gmap_t> const & clockbounds)
    : _clockbounds(clockbounds), _vloc_to_bounds(*_clockbounds)
{
}

node_le_t::node_le_t(tchecker::gta::system_t const & system)
    : _clockbounds(std::make_shared<tchecker::clockbounds::gta::loc_gmap_t>(tchecker::clockbounds::gta::compute_gmap(system))),
      _vloc_to_bounds(*_clockbounds)
{
}

bool node_le_t::operator()(tchecker::tck_reach::gtacovreach::node_t const & n1,
                           tchecker::tck_reach::gtacovreach::node_t const & n2) const
{
  return tchecker::gtazg::shared_is_gmap_le(n1.state(), n2.state(), _vloc_to_bounds);
}

/* edge_t */

edge_t::edge_t(tchecker::gtazg::transition_t const & t) : tchecker::graph::edge_vedge_t(t.vedge_ptr()) {}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::gtazg::gtazg_t> const & gtazg, std::size_t block_size, std::size_t table_size)
    : tchecker::graph::subsumption::graph_t<tchecker::tck_reach::gtacovreach::node_t, tchecker::tck_reach::gtacovreach::edge_t,
                                            tchecker::tck_reach::gtacovreach::node_hash_t,
                                            tchecker::tck_reach::gtacovreach::node_le_t>(
          block_size, table_size, tchecker::tck_reach::gtacovreach::node_hash_t(),
          tchecker::tck_reach::gtacovreach::node_le_t(gtazg->system())),
      _gtazg(gtazg)
{
}

graph_t::~graph_t()
{
  tchecker::graph::subsumption::graph_t<tchecker::tck_reach::gtacovreach::node_t, tchecker::tck_reach::gtacovreach::edge_t,
                                        tchecker::tck_reach::gtacovreach::node_hash_t,
                                        tchecker::tck_reach::gtacovreach::node_le_t>::clear();
}

void graph_t::attributes(tchecker::tck_reach::gtacovreach::node_t const & n, std::map<std::string, std::string> & m) const
{
  _gtazg->attributes(n.state_ptr(), m);
  tchecker::graph::attributes(static_cast<tchecker::graph::node_flags_t const &>(n), m);
}

void graph_t::attributes(tchecker::tck_reach::gtacovreach::edge_t const & e, std::map<std::string, std::string> & m) const
{
  m["vedge"] = tchecker::to_string(e.vedge(), _gtazg->system().as_system_system());
}

bool graph_t::is_actual_edge(edge_sptr_t const & e) const { return edge_type(e) == tchecker::graph::subsumption::EDGE_ACTUAL; }

/* dot_output */

/*!
 \class node_lexical_less_t
 \brief Less-than order on nodes based on lexical ordering
*/
class node_lexical_less_t {
public:
  /*!
   \brief Less-than order on nodes based on lexical ordering
   \param n1 : a node
   \param n2 : a node
   \return true if n1 is less-than n2 w.r.t. lexical ordering over the states in
   the nodes
  */
  bool operator()(tchecker::tck_reach::gtacovreach::graph_t::node_sptr_t const & n1,
                  tchecker::tck_reach::gtacovreach::graph_t::node_sptr_t const & n2) const
  {
    int state_cmp = tchecker::gtazg::lexical_cmp(n1->state(), n2->state());
    if (state_cmp != 0)
      return (state_cmp < 0);
    return (tchecker::graph::lexical_cmp(static_cast<tchecker::graph::node_flags_t const &>(*n1),
                                         static_cast<tchecker::graph::node_flags_t const &>(*n2)) < 0);
  }
};

/*!
 \class edge_lexical_less_t
 \brief Less-than ordering on edges based on lexical ordering
 */
class edge_lexical_less_t {
public:
  /*!
   \brief Less-than ordering on edges based on lexical ordering
   \param e1 : an edge
   \param e2 : an edge
   \return true if e1 is less-than  e2 w.r.t. the tuple of edges in e1 and e2
  */
  bool operator()(tchecker::tck_reach::gtacovreach::graph_t::edge_sptr_t const & e1,
                  tchecker::tck_reach::gtacovreach::graph_t::edge_sptr_t const & e2) const
  {
    return tchecker::lexical_cmp(e1->vedge(), e2->vedge()) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::tck_reach::gtacovreach::graph_t const & g, std::string const & name)
{
  return tchecker::graph::subsumption::dot_output<tchecker::tck_reach::gtacovreach::graph_t,
                                                  tchecker::tck_reach::gtacovreach::node_lexical_less_t,
                                                  tchecker::tck_reach::gtacovreach::edge_lexical_less_t>(os, g, name);
}

/* run */

std::tuple<tchecker::algorithms::covreach::stats_t, std::shared_ptr<tchecker::tck_reach::gtacovreach::graph_t>>
run(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & labels, std::string const & search_order,
    tchecker::algorithms::covreach::covering_t covering, std::size_t block_size, std::size_t table_size)
{
  std::shared_ptr<tchecker::gta::system_t const> system{new tchecker::gta::system_t{sysdecl}};
  if (!tchecker::system::every_process_has_initial_location(system->as_system_system())) {
    std::cerr << tchecker::log_warning << "system has no initial state" << std::endl;
  }

  std::shared_ptr<tchecker::gtazg::gtazg_t> gtazg{
      tchecker::gtazg::factory(system, tchecker::ts::SHARING, block_size, table_size)};

  std ::shared_ptr<tchecker::tck_reach::gtacovreach::graph_t> graph{
      new tchecker::tck_reach::gtacovreach::graph_t{gtazg, block_size, table_size}};

  boost::dynamic_bitset<> accepting_labels = system->as_syncprod_system().labels(labels);

  enum tchecker::waiting::policy_t policy = tchecker::algorithms::fast_remove_waiting_policy(search_order);

  tchecker::algorithms::covreach::stats_t stats;
  tchecker::tck_reach::gtacovreach::algorithm_t algorithm;

  if (covering == tchecker::algorithms::covreach::COVERING_FULL)
    stats = algorithm.run<tchecker::algorithms::covreach::COVERING_FULL>(*gtazg, *graph, accepting_labels, policy);
  else if (covering == tchecker::algorithms::covreach::COVERING_LEAF_NODES)
    stats = algorithm.run<tchecker::algorithms::covreach::COVERING_LEAF_NODES>(*gtazg, *graph, accepting_labels, policy);
  else
    throw std::invalid_argument("Unknown covering policy for covreach algorithm");

  return std::make_tuple(stats, graph);
}

} // end of namespace gtacovreach

} // end of namespace tck_reach

} // end of namespace tchecker
