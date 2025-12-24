/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_COVREACH_ALGORITHM_HH
#define TCHECKER_GTA_COVREACH_ALGORITHM_HH

#include <memory>
#include <string>

#include "tchecker/algorithms/covreach/algorithm.hh"
#include "tchecker/algorithms/covreach/stats.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/graph/edge.hh"
#include "tchecker/graph/node.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/graph/subsumption_graph.hh"
#include "tchecker/gta/system.hh"
#include "tchecker/gtazg/gtazg.hh"
#include "tchecker/gtazg/state.hh"
#include "tchecker/gtazg/transition.hh"
#include "tchecker/waiting/waiting.hh"

/*!
 \file gta-covreach.hh
 \brief Covering reachability algorithm over the gta zone graph
*/

namespace tchecker {

namespace tck_reach {

namespace gtacovreach {

/*!
 \class node_t
 \brief Node of the subsumption graph over the gta zone graph
 */
class node_t : public tchecker::waiting::element_t,
               public tchecker::graph::node_flags_t,
               public tchecker::graph::node_gtazg_state_t {
public:
  /*!
   \brief Constructor
   \param s : a state of the local-time zone graph
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(tchecker::gtazg::state_sptr_t const & s, bool initial = false, bool final = false);

  /*!
   \brief Constructor
   \param s : a state of gta zone graph
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(tchecker::gtazg::const_state_sptr_t const & s, bool initial = false, bool final = false);
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
  \return hash value for n based on the discrete part of n (i.e. the tuple of
  locations and integer variable valuations) since we need to cover nodes with
  same discrete part
  */
  std::size_t operator()(tchecker::tck_reach::gtacovreach::node_t const & n) const;
};

/*!
\class node_le_t
\brief Covering predicate for nodes
*/
class node_le_t {
public:
  /*!
  \brief Constructor
  \param clockbounds : clockbounds
  \note this keeps a shared pointer on clockbounds
  */
  node_le_t(std::shared_ptr<tchecker::clockbounds::gta::loc_gmap_t> const & clockbounds);

  /*!
  \brief Constructor
  \param system : a system of generalized timed processes
  \note this computes the clock bounds on system
  \throw std::runtime_error : if clock bounds cannot be computed for system
  */
  node_le_t(tchecker::gta::system_t const & system);

  /*!
  \brief Copy constructor
  */
  node_le_t(tchecker::tck_reach::gtacovreach::node_le_t const & node_le) = default;

  /*!
  \brief Move constructor
  */
  node_le_t(tchecker::tck_reach::gtacovreach::node_le_t && node_le) = default;

  /*!
   \brief Destructor
  */
  ~node_le_t() = default;

  /*!
  \brief Covering predicate for nodes
  \param n1 : a node
  \param n2 : a node
  \return true if n1 and n2 have same discrete part and the zone of n1 is
  sync-aLU subsumed in the zone of n2, false otherwise
  */
  bool operator()(tchecker::tck_reach::gtacovreach::node_t const & n1,
                  tchecker::tck_reach::gtacovreach::node_t const & n2) const;

private:
  std::shared_ptr<tchecker::clockbounds::gta::loc_gmap_t> _clockbounds;
  tchecker::clockbounds::gta::vloc_to_bounds_t _vloc_to_bounds;
};

/*!
 \class edge_t
 \brief Edge of the subsumption graph of a gta zone graph
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
 \brief Subsumption graph over the local-time zone graph
*/
class graph_t : public tchecker::graph::subsumption::graph_t<
                    tchecker::tck_reach::gtacovreach::node_t, tchecker::tck_reach::gtacovreach::edge_t,
                    tchecker::tck_reach::gtacovreach::node_hash_t, tchecker::tck_reach::gtacovreach::node_le_t> {
public:
  /*!
   \brief Constructor
   \param gtazg : gta zone graph
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a shared pointer on gtazg
   \throw std::runtime_error : if clock bounds cannot be computed for the underlying system in gtazg
  */
  graph_t(std::shared_ptr<tchecker::gtazg::gtazg_t> const & gtazg, std::size_t block_size, std::size_t table_size);

  /*!
   \brief Destructor
  */
  virtual ~graph_t();

  /*!
   \brief Accessor
   \return Shared pointer to underlying zone graph with reference clocks
  */
  inline std::shared_ptr<tchecker::gtazg::gtazg_t> const & gtazg_ptr() const { return _gtazg; }

  /*!
   \brief Accessor
   \return Underlying zone graph with reference clocks
  */
  inline tchecker::gtazg::gtazg_t const & gtazg() const { return *_gtazg; }

  using tchecker::graph::subsumption::graph_t<
      tchecker::tck_reach::gtacovreach::node_t, tchecker::tck_reach::gtacovreach::edge_t,
      tchecker::tck_reach::gtacovreach::node_hash_t, tchecker::tck_reach::gtacovreach::node_le_t>::attributes;

  /*!
   \brief Checks is an edge is an actual edge (not a subsumption edge)
   \param e : an edge
   \return true if e is an actual edge, false otherwise (e is a subsumption edge)
   */
  bool is_actual_edge(edge_sptr_t const & e) const;

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  virtual void attributes(tchecker::tck_reach::gtacovreach::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  virtual void attributes(tchecker::tck_reach::gtacovreach::edge_t const & e, std::map<std::string, std::string> & m) const;

private:
  std::shared_ptr<tchecker::gtazg::gtazg_t> _gtazg; /*!< GTA Zone graph */
};

std::ostream & dot_output(std::ostream & os, tchecker::tck_reach::gtacovreach::graph_t const & g, std::string const & name);

/*!
 \class algorithm_t
 \brief Covering reachability algorithm over the local-time zone graph
*/
class algorithm_t
    : public tchecker::algorithms::covreach::algorithm_t<tchecker::gtazg::gtazg_t, tchecker::tck_reach::gtacovreach::graph_t> {
public:
  using tchecker::algorithms::covreach::algorithm_t<tchecker::gtazg::gtazg_t,
                                                    tchecker::tck_reach::gtacovreach::graph_t>::algorithm_t;
};

/*!
 \brief Run covering reachability algorithm on the GTA zone graph of a
 system
 \param sysdecl : system declaration
 \param labels : comma-separated string of labels
 \param search_order : search order
 \param covering : covering policy
 \param block_size : number of elements allocated in one block
 \param table_size : size of hash tables
 \pre labels must appear as node attributes in sysdecl
 search_order must be either "dfs" or "bfs"
 \return statistics on the run and the covering reachability graph
 \throw std::runtime_error : if clock bounds cannot be computed for the system modeled as sysdecl
 */
std::tuple<tchecker::algorithms::covreach::stats_t, std::shared_ptr<tchecker::tck_reach::gtacovreach::graph_t>>
run(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & labels = "",
    std::string const & search_order = "bfs",
    tchecker::algorithms::covreach::covering_t covering = tchecker::algorithms::covreach::COVERING_FULL,
    std::size_t block_size = 10000, std::size_t table_size = 65536);

} // end of namespace gtacovreach

} // end of namespace tck_reach

} // end of namespace tchecker

#endif // TCHECKER_GTA_COVREACH_ALGORITHM_HH
