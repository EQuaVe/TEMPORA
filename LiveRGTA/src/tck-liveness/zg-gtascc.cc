/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset.hpp>

#include "tchecker/clockbounds/solver.hh"
#include "tchecker/gtazg/state.hh"
#include "tchecker/system/static_analysis.hh"
#include "tchecker/utils/log.hh"
#include "zg-gtascc.hh"

namespace tchecker {

namespace tck_liveness {

namespace zg_gtascc {

namespace algorithm {

/*!
 \class generalized_algorithm_t
 \brief GTA scc decomposition based liveness algorithm over the zone graph with generalized Büchi conditions
*/
class generalized_algorithm_t {
public:
  using node_sptr_t = typename tchecker::tck_liveness::zg_gtascc::graph_t::node_sptr_t;
  using edge_sptr_t = typename tchecker::gtazg::transition_sptr_t;

  /*!
   \brief Check if a transition has an infinite run that satisfies a given set
   of labels and build the corresponding graph
   \param zg : a gta zone graph
   \param graph : an empty graph for liveness
   \param labels : accepting labels
   \post graph is built from a traversal of zg starting from its initial states,
   until a cycle that satisfies labels and can be taken infinitely often is reached (if any).
   A node is created for each reached state in zg, and an edge is created for
   each transition in zg.
   Initial nodes have been marked in graph
   \return statistics on the run
   */
  tchecker::algorithms::gtascc::stats_t run(tchecker::gtazg::gtazg_t & zg, tchecker::tck_liveness::zg_gtascc::graph_t & graph,
                                            boost::dynamic_bitset<> const & labels)
  {
    tchecker::algorithms::couvscc::stats_t stats;

    stats.set_start_time();

    _count = 0;
    _num_clks = zg.system().clocks_count(tchecker::VK_FLATTENED);
    boost::dynamic_bitset<> const & prophecy_clks = zg.system().prophecy_clks();

    std::vector<typename tchecker::gtazg::gtazg_t::sst_t> sst;
    zg.initial(sst);
    for (auto && [status, s, t] : sst) {
      auto && [is_new_node, initial_node] = graph.add_node(s);
      initial_node->initial(true);
      couv_dfs(initial_node, zg, graph, labels, prophecy_clks, stats);
      if (stats.cycle()) {
        break;
      }
    }

    stats.stored_states() = graph.nodes_count();

    empty_stacks();

    stats.set_end_time();

    return stats;
  }

private:
  /*!
   \brief DFS loop of GTA SCC algorithm
   \param n : a node
   \param zg : a gta zone graph
   \param graph : a graph
   \param labels : accepting labels
   \param prophecy_clks : the set of prophecy clocks
   \param stats : statistics on the run
   \post the DFS search in GTA SCC algorithm has been performed from n.
   stats.cycle() is true if an accepting cycle w.r.t labels which can be taken infinitely often has been found in
   zg, and false otherwise
   graph contains the part of ts that has been explored
  */
  void couv_dfs(node_sptr_t & n, tchecker::gtazg::gtazg_t & zg, tchecker::tck_liveness::zg_gtascc::graph_t & graph,
                boost::dynamic_bitset<> const & labels, boost::dynamic_bitset<> const & prophecy_clks,
                tchecker::algorithms::couvscc::stats_t & stats)
  {
    push(n, boost::dynamic_bitset<>(_num_clks), zg, graph, stats);
    while (!_todo.empty()) {
      auto && [n, succ] = _todo.top();
      if (succ.empty()) {
        if (_roots.top().n == n) {
          close_scc(n);
        }
        _todo.pop();
      }
      else {
        auto && [t, next_node] = _todo.top().pick_successor();
        if (next_node->dfsnum() == 0) {
          boost::dynamic_bitset<> released_clks = zg.released_clks(t->vedge());
          push(next_node, released_clks, zg, graph, stats);
        }
        else if (next_node->current()) {
          boost::dynamic_bitset<> released_clks = zg.released_clks(t->vedge());
          _transition_released_clks.push(released_clks);
          merge_scc(next_node, zg, stats, labels, prophecy_clks);
          if (stats.cycle()) {
            break;
          }
        }
      }
    }
  }

  /*!
   \brief Close the SCC of node n
   \param n : a node
   \pre n is a root node
   \post the current flag of all nodes in the SCC of n has been set to false
   (hence closing the SCC)
  */
  void close_scc(node_sptr_t & n)
  {
    _roots.pop();
    _transition_released_clks.pop();

    node_sptr_t u{nullptr};
    do {
      u = _active.top();
      _active.pop();
      u->current() = false;
    } while (u != n);
  }

  /*!
   \brief Merge the SCCs on a loop (generalised Büchi conditions)
   \param n : a node
   \param zg : GTA zone graph
   \stats :  statistics
   \param labels : accepting labels
   \param prophecy_clks : set of prophecy clocks
   \post all SCCs on a loop on n have been merged
   stats.cycle() is true if accepting cycle found
  */
  void merge_scc(node_sptr_t & n, tchecker::gtazg::gtazg_t & zg, tchecker::algorithms::couvscc::stats_t & stats,
                 boost::dynamic_bitset<> const & labels, boost::dynamic_bitset<> const & prophecy_clks)
  {
    boost::dynamic_bitset<> scc_labels{labels.size(), 0};
    boost::dynamic_bitset<> released_clks{_num_clks, 0};
    node_sptr_t u{_roots.top().n};
    do {
      released_clks |= _transition_released_clks.top();
      _transition_released_clks.pop();

      u = _roots.top().n;
      scc_labels |= _roots.top().labels;
      released_clks |= _roots.top().released_clks;
      _roots.pop();

    } while (u->dfsnum() > n->dfsnum());

    _roots.push({u, scc_labels, released_clks});

    if (labels.is_subset_of(scc_labels)) {
      boost::dynamic_bitset<> const minus_inf_clks = zg.minus_inf_clks(u->state_ptr());
      if ((released_clks | minus_inf_clks) == prophecy_clks) {
        stats.cycle() = true;
        return;
      }
    }
  }

  /*!
   \brief Push node and set its flags
   \param n : a node
   \param released_clks : set of prophecy clocks released in transition to n
   \param zg : a gta zone graph
   \param graph : a graph
   \param stats : statistics
   \post n has been expanded into graph, its flags (current and dfsnum) have
   been set, and n has been pushed to the stacks todo, roots and active and released_clks has been push to
   transition_released_clks
   a visited node has been added to stats
   */
  void push(node_sptr_t & n, boost::dynamic_bitset<> const released_clks, tchecker::gtazg::gtazg_t & zg,
            tchecker::tck_liveness::zg_gtascc::graph_t & graph, tchecker::algorithms::couvscc::stats_t & stats)
  {
    ++_count;
    n->dfsnum() = _count;
    n->current() = true;
    _todo.push(todo_stack_entry_t{n, expand_node(n, zg, graph, stats)});
    _roots.push(roots_stack_entry_t{n, zg.labels(n->state_ptr()), boost::dynamic_bitset<>(_num_clks)});
    _active.push(n);
    _transition_released_clks.push(released_clks);
    ++stats.visited_states();
  }

  /*!
   \brief Adds successor nodes to the graph
   \param n : a node
   \param zg : a gta zone graph
   \param graph : a graph
   \param stats : statistics
   \post all successor nodes of n in ts have been added to graph (if not yet in) with corresponding edges
   visited transitions have been added to stats
   \return all successor nodes of n
  */
  std::deque<std::pair<edge_sptr_t, node_sptr_t>> expand_node(node_sptr_t & n, tchecker::gtazg::gtazg_t & zg,
                                                              tchecker::tck_liveness::zg_gtascc::graph_t & graph,
                                                              tchecker::algorithms::couvscc::stats_t & stats)
  {
    std::deque<std::pair<edge_sptr_t, node_sptr_t>> next_nodes;
    std::vector<tchecker::gtazg::gtazg_t::sst_t> v;
    zg.next(n->state_ptr(), v);
    for (auto && [status, s, t] : v) {
      ++stats.visited_transitions();
      auto && [new_node, nextn] = graph.add_node(s);
      graph.add_edge(n, nextn, *t);
      next_nodes.push_back(std::make_pair(t, nextn));
    }
    return next_nodes;
  }

  /*!
   \class todo_stack_entry_t
   \brief Entry of the todo stack
   */
  struct todo_stack_entry_t {
    node_sptr_t n;                                        /*!< Current node */
    std::deque<std::pair<edge_sptr_t, node_sptr_t>> succ; /*!< Successor nodes */

    /*!
     \brief Remove and return the first successor node
     \pre succ is not empty (checked by assertion)
     \return the first successor of node n
     \post the first successors of node n has been removed from succ
    */
    std::pair<edge_sptr_t, node_sptr_t> pick_successor()
    {
      assert(!succ.empty());
      std::pair<edge_sptr_t, node_sptr_t> n = succ.front();
      succ.pop_front();
      return n;
    }
  };

  /*!
   \class roots_stack_entry_t
   \brief Entry of the roots stack
  */
  struct roots_stack_entry_t {
    node_sptr_t n;                         /*!< Root node */
    boost::dynamic_bitset<> labels;        /*!< Labels in the explored SCC of n */
    boost::dynamic_bitset<> released_clks; /*!< Set of prophecy clocks released in SCC of n */
  };

  /*!
   \brief Empty all stacks
   \post All stacks are empty
   */
  void empty_stacks()
  {
    while (!_todo.empty())
      _todo.pop();
    while (!_roots.empty())
      _roots.pop();
    while (!_active.empty())
      _active.pop();
    while (!_transition_released_clks.empty())
      _transition_released_clks.pop();
  }

private:
  tchecker::clock_id_t _num_clks;
  unsigned int _count;                                           /*!< DFS number counter */
  std::stack<todo_stack_entry_t> _todo;                          /*!< todo stack */
  std::stack<roots_stack_entry_t> _roots;                        /*!< roots stack */
  std::stack<node_sptr_t> _active;                               /*!< active stack */
  std::stack<boost::dynamic_bitset<>> _transition_released_clks; /*!< stack of clocks released in transition */
};
} // end of namespace algorithm

/* node_t */

node_t::node_t(tchecker::gtazg::state_sptr_t const & s) : tchecker::graph::node_gtazg_state_t(s) {}

node_t::node_t(tchecker::gtazg::const_state_sptr_t const & s) : tchecker::graph::node_gtazg_state_t(s) {}

/* node_hash_t */

std::size_t node_hash_t::operator()(tchecker::tck_liveness::zg_gtascc::node_t const & n) const
{
  // We hash only on the discrete part and lower bounds of prophecy clocks, since the equality function on zones is an
  // equivalence relation, where the zones need not be equal
  std::size_t hash = tchecker::gtazg::shared_partial_hash_value(n.state());
  return hash;
}

/* node_equal_to_t */

node_equal_to_t::node_equal_to_t(tchecker::gta::system_t const & system)
    : _clockbounds(std::make_shared<tchecker::clockbounds::gta::loc_gmap_t>(tchecker::clockbounds::gta::compute_gmap(system))),
      _vloc_to_bounds(*_clockbounds)
{
}

bool node_equal_to_t::operator()(tchecker::tck_liveness::zg_gtascc::node_t const & n1,
                                 tchecker::tck_liveness::zg_gtascc::node_t const & n2) const
{
  if (not tchecker::ta::shared_equal_to(n1.state(), n2.state())) {
    return false;
  }

  if (n1.state().zone_ptr() == n2.state().zone_ptr()) {
    return true;
  }

  tchecker::clockbounds::gta::gsim_map_t & gsim_map = _vloc_to_bounds.bounds_of_vloc(n1.state().vloc_ptr());
  tchecker::gtazg::gta_zone_t const & zone1 = n1.state().zone();
  tchecker::gtazg::gta_zone_t const & zone2 = n2.state().zone();

  return (tchecker::gtazg::is_mutual_sim(zone1, zone2, gsim_map));
}

/* edge_t */

edge_t::edge_t(tchecker::gtazg::transition_t const & t) : tchecker::graph::edge_vedge_t(t.vedge_ptr()) {}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::gtazg::gtazg_t const> const & zg, std::size_t block_size, std::size_t table_size)
    : tchecker::graph::reachability::graph_t<
          tchecker::tck_liveness::zg_gtascc::node_t, tchecker::tck_liveness::zg_gtascc::edge_t,
          tchecker::tck_liveness::zg_gtascc::node_hash_t, tchecker::tck_liveness::zg_gtascc::node_equal_to_t>(
          block_size, table_size, tchecker::tck_liveness::zg_gtascc::node_hash_t(),
          tchecker::tck_liveness::zg_gtascc::node_equal_to_t(zg->system())),
      _zg(zg)
{
}

graph_t::~graph_t()
{
  tchecker::graph::reachability::graph_t<tchecker::tck_liveness::zg_gtascc::node_t, tchecker::tck_liveness::zg_gtascc::edge_t,
                                         tchecker::tck_liveness::zg_gtascc::node_hash_t,
                                         tchecker::tck_liveness::zg_gtascc::node_equal_to_t>::clear();
}

void graph_t::attributes(tchecker::tck_liveness::zg_gtascc::node_t const & n, std::map<std::string, std::string> & m) const
{
  _zg->attributes(n.state_ptr(), m);
  tchecker::graph::attributes(static_cast<tchecker::graph::node_flags_t const &>(n), m);
}

void graph_t::attributes(tchecker::tck_liveness::zg_gtascc::edge_t const & e, std::map<std::string, std::string> & m) const
{
  m["vedge"] = tchecker::to_string(e.vedge(), _zg->system().as_system_system());
}

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
  bool operator()(tchecker::tck_liveness::zg_gtascc::graph_t::node_sptr_t const & n1,
                  tchecker::tck_liveness::zg_gtascc::graph_t::node_sptr_t const & n2) const
  {
    int state_cmp = tchecker::gtazg::lexical_cmp(n1->state(), n2->state());
    if (state_cmp != 0) {
      return (state_cmp < 0);
    }
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
   \return true if e1 is less-than e2 w.r.t. the tuple of edges in e1 and e2
  */
  bool operator()(tchecker::tck_liveness::zg_gtascc::graph_t::edge_sptr_t const & e1,
                  tchecker::tck_liveness::zg_gtascc::graph_t::edge_sptr_t const & e2) const
  {
    return tchecker::lexical_cmp(e1->vedge(), e2->vedge()) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::tck_liveness::zg_gtascc::graph_t const & g, std::string const & name)
{
  return tchecker::graph::reachability::dot_output<tchecker::tck_liveness::zg_gtascc::graph_t,
                                                   tchecker::tck_liveness::zg_gtascc::node_lexical_less_t,
                                                   tchecker::tck_liveness::zg_gtascc::edge_lexical_less_t>(os, g, name);
}

/* run */

std::tuple<tchecker::algorithms::gtascc::stats_t, std::shared_ptr<tchecker::tck_liveness::zg_gtascc::graph_t>>
run(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & labels, std::size_t block_size,
    std::size_t table_size)
{
  std::shared_ptr<tchecker::gta::system_t const> system{new tchecker::gta::system_t{sysdecl}};
  if (!tchecker::system::every_process_has_initial_location(system->as_system_system()))
    std::cerr << tchecker::log_warning << "system has no initial state" << std::endl;

  std::shared_ptr<tchecker::gtazg::gtazg_t> zg{tchecker::gtazg::factory(system, tchecker::ts::SHARING, block_size, table_size)};

  std::shared_ptr<tchecker::tck_liveness::zg_gtascc::graph_t> graph{
      new tchecker::tck_liveness::zg_gtascc::graph_t{zg, block_size, table_size}};

  boost::dynamic_bitset<> accepting_labels = system->as_syncprod_system().labels(labels);

  tchecker::algorithms::gtascc::stats_t stats;

  tchecker::tck_liveness::zg_gtascc::algorithm::generalized_algorithm_t algorithm;
  stats = algorithm.run(*zg, *graph, accepting_labels);
  return std::make_tuple(stats, graph);
}
} // end of namespace zg_gtascc

} // end of namespace tck_liveness

} // end of namespace tchecker
