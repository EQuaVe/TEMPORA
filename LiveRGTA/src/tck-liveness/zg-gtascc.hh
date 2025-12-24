/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_GTA_SCC_ALGORITHM_HH
#define TCHECKER_ZG_GTA_SCC_ALGORITHM_HH

#include "tchecker/algorithms/gta_scc/graph.hh"
#include "tchecker/algorithms/gta_scc/stats.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/graph/edge.hh"
#include "tchecker/graph/node.hh"
#include "tchecker/graph/reachability_graph.hh"
#include "tchecker/gtazg/gtazg.hh"
#include "tchecker/gtazg/state.hh"
#include "tchecker/gtazg/transition.hh"
#include <deque>
#include <stack>

namespace tchecker {

namespace tck_liveness {

namespace zg_gtascc {

/*!
 \class node_t
 \brief Node of the liveness graph of a gta zone graph
 */
class node_t : public tchecker::algorithms::gtascc::node_t, public tchecker::graph::node_gtazg_state_t {
public:
  /*!
   \brief Constructor
   \param s : a gta zone graph state
   \post this node keeps a shared pointer to s
   */
  node_t(tchecker::gtazg::state_sptr_t const & s);

  /*!
   \brief Constructor
   \param s : a gta zone graph state
   \post this node keeps a shared pointer to s
   */
  node_t(tchecker::gtazg::const_state_sptr_t const & s);
};

/*!
\class node_hash_t
\brief Hash functor for nodes
*/
class node_hash_t {
public:
  /*!
  \brief Hash function
  \param n : a node
  \return hash value for n
  */
  std::size_t operator()(tchecker::tck_liveness::zg_gtascc::node_t const & n) const;
};

/*!
\class node_equal_to_t
\brief Equality check functor for nodes
*/
class node_equal_to_t {
public:
  node_equal_to_t(tchecker::gta::system_t const & system);
  /*!
  \brief Equality predicate
  \param n1 : a node
  \param n2 : a node
  \return true if n1 and n2 have the same discrete states and the zones are bi-simulated with respect to gmap
  */
  bool operator()(tchecker::tck_liveness::zg_gtascc::node_t const & n1,
                  tchecker::tck_liveness::zg_gtascc::node_t const & n2) const;

private:
  std::shared_ptr<tchecker::clockbounds::gta::loc_gmap_t> const _clockbounds;
  tchecker::clockbounds::gta::vloc_to_bounds_t const _vloc_to_bounds;
};

/*!
 \class edge_t
 \brief Edge of the liveness graph of a gta zone graph
*/
class edge_t : public tchecker::graph::edge_vedge_t {
public:
  /*!
   \brief Constructor
   \param t : a gta zone graph transition
   \post this node keeps a shared pointer on the vedge in t
  */
  edge_t(tchecker::gtazg::transition_t const & t);
};

/*!
 \class graph_t
 \brief Liveness graph over the gta zone graph
*/
class graph_t : public tchecker::graph::reachability::graph_t<
                    tchecker::tck_liveness::zg_gtascc::node_t, tchecker::tck_liveness::zg_gtascc::edge_t,
                    tchecker::tck_liveness::zg_gtascc::node_hash_t, tchecker::tck_liveness::zg_gtascc::node_equal_to_t> {
public:
  /*!
   \brief Constructor
   \param zg : gta zone graph
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a pointer on zg
  */
  graph_t(std::shared_ptr<tchecker::gtazg::gtazg_t const> const & zg, std::size_t block_size, std::size_t table_size);

  /*!
   \brief Destructor
  */
  virtual ~graph_t();

  using tchecker::graph::reachability::graph_t<
      tchecker::tck_liveness::zg_gtascc::node_t, tchecker::tck_liveness::zg_gtascc::edge_t,
      tchecker::tck_liveness::zg_gtascc::node_hash_t, tchecker::tck_liveness::zg_gtascc::node_equal_to_t>::attributes;

  /*!
   \brief Accessor
   \return underlying gta zone graph
  */
  inline tchecker::gtazg::gtazg_t const & zg() const { return *_zg; }

  /*!
   \brief Accessor
   \return shared pointer to underlying zone graph
  */
  inline std::shared_ptr<tchecker::gtazg::gtazg_t const> zg_ptr() const { return _zg; }

  /*!
   \brief Checks if an edge is an actual edge in the graph
   \return true (all edges are actual edges in the reachability graph)
   */
  bool is_actual_edge(edge_sptr_t const &) const { return true; }

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  virtual void attributes(tchecker::tck_liveness::zg_gtascc::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  virtual void attributes(tchecker::tck_liveness::zg_gtascc::edge_t const & e, std::map<std::string, std::string> & m) const;

private:
  std::shared_ptr<tchecker::gtazg::gtazg_t const> _zg; /*!< Zone graph */
};

/*!
 \brief Graph output
 \param os : output stream
 \param g : graph
 \param name : graph name
 \post graph g with name has been output to os
*/
std::ostream & dot_output(std::ostream & os, tchecker::tck_liveness::zg_gtascc::graph_t const & g, std::string const & name);

/*!
 \brief Run GTA scc algorithm on the zone graph of a system
 \param sysdecl : system declaration
 \param labels : comma-separated string of labels
 \param block_size : number of elements allocated in one block
 \param table_size : size of hash tables
 \pre labels must appear as node attributes in sysdecl
 \return statistics on the run and the liveness graph
 */
std::tuple<tchecker::algorithms::gtascc::stats_t, std::shared_ptr<tchecker::tck_liveness::zg_gtascc::graph_t>>
run(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & labels = "", std::size_t block_size = 10000,
    std::size_t table_size = 65536);

} // end of namespace zg_gtascc

} // namespace tck_liveness

} // end of namespace tchecker

#endif // TCHECKER_ZG_GTA_SCC_ALGORITHM_HH
