/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTAZG_HH
#define TCHECKER_GTAZG_HH

#include <cstdlib>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/gtadbm.hh"
#include "tchecker/gta/gta.hh"
#include "tchecker/gta/system.hh"
#include "tchecker/gtazg/allocators.hh"
#include "tchecker/gtazg/gta_zone.hh"
#include "tchecker/gtazg/semantics.hh"
#include "tchecker/gtazg/state.hh"
#include "tchecker/gtazg/transition.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ts/fwd.hh"
#include "tchecker/ts/inspector.hh"
#include "tchecker/ts/sharing.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file zg.hh
 \brief Zone graphs
 */

namespace tchecker {

namespace gtazg {

// Initial edges

/*!
 \brief Type of iterator over initial states
 */
using initial_iterator_t = tchecker::gta::initial_iterator_t;

/*!
\brief Type of range of iterators over inital states
*/
using initial_range_t = tchecker::gta::initial_range_t;

/*!
 \brief Accessor to initial edges
 \param system : a system
 \return initial edges
 */
inline tchecker::gtazg::initial_range_t initial_edges(tchecker::gta::system_t const & system)
{
  return tchecker::gta::initial_edges(system);
}

/*!
 \brief Dereference type for iterator over initial states
 */
using initial_value_t = tchecker::gta::initial_value_t;

// Initial states

/*!
 \brief Compute initial state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param semantics : a zone semantics
 \param initial_range : range of initial state valuations
 \pre the size of vloc and vedge is equal to the size of initial_range.
 initial_range has been obtained from system.
 initial_range yields the initial locations of all the processes ordered by increasing process identifier
 \post vloc has been initialized to the tuple of initial locations in initial_range,
 intval has been initialized to the initial valuation of bounded integer variables,
 vedge has been initialized to an empty tuple of edges.
 sync_id has been set to tchecker::NO_SYNC.
 zone has been initialized to the initial set of clock valuations according to
 semantics and declaration
 \return tchecker::STATE_OK if initialization succeeded,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the initial valuation of integer
 variables does not satisfy invariant
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t initial(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                 tchecker::intval_sptr_t const & intval, tchecker::gtazg::gta_zone_sptr_t const & zone,
                                 tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                                 tchecker::gtazg::standard_semantics_t & semantics,
                                 tchecker::gtazg::initial_value_t const & initial_range);

/*!
 \brief Compute initial state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param v : initial iterator value
 \return tchecker::STATE_OK if initialization of s and t succeeded, see
 tchecker::gtazg::initial for returned values when initialization fails
 \throw std::invalid_argument : if s and v have incompatible sizes
*/
inline tchecker::state_status_t initial(tchecker::gta::system_t const & system, tchecker::gtazg::state_t & s,
                                        tchecker::gtazg::transition_t & t, tchecker::gtazg::standard_semantics_t & semantics,
                                        tchecker::gtazg::initial_value_t const & v)
{
  return tchecker::gtazg::initial(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.sync_id(), semantics, v);
}

// Outgoing edges

/*!
 \brief Type of iterator over outgoing edges
 */
using outgoing_edges_iterator_t = tchecker::gta::outgoing_edges_iterator_t;

/*!
\brief Type of range of outgoing edges
*/
using outgoing_edges_range_t = tchecker::gta::outgoing_edges_range_t;

/*!
 \brief Accessor to outgoing edges
 \param system : a system
 \param vloc : tuple of locations
 \return range of outgoing synchronized and asynchronous edges from vloc in system
 */
inline tchecker::gtazg::outgoing_edges_range_t outgoing_edges(tchecker::gta::system_t const & system,
                                                              tchecker::const_vloc_sptr_t const & vloc)
{
  return tchecker::gta::outgoing_edges(system, vloc);
}

/*!
 \brief Type of outgoing vedge (range of synchronized/asynchronous edges)
 */
using outgoing_edges_value_t = tchecker::gta::outgoing_edges_value_t;

// Next states

/*!
 \brief Compute next state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param semantics : a zone semantics
 \param sync_edges : tuple of edge from vloc
 \pre the source location of edges in sync_edges match the locations in vloc.
 No process has more than one edge in sync_edges.
 The pid of every process involved in sync_edges is less than the size of vloc
 \post the locations in vloc have been updated to target locations of the
 processes involved in sync_edges, and they have been left unchanged for the other
 processes.
 The values of variables in intval have been updated according to the statements
 of the edges in sync_edges.
 The zone has been updated according to semantics.
 vedge is the tuple of edges in sync_edges.
 sync_id has been set to tchecker::NO_SYNC
 \return tchecker::STATE_OK if state computation succeeded,
 tchecker::STATE_INCOMPATIBLE_EDGE if the source locations of the edges in sync_edges do not match
 vloc,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the valuation intval does not
 satisfy the invariant in vloc,
 tchecker::STATE_INTVARS_GUARD_VIOLATED if the values in intval do not satisfy
 the guard of the edges in sync_edges,
 tchecker::STATE_CLOCKS_GUARD_VIOLATED if the zone updated w.r.t. src_invariant
 does not satisfy the guard of the GTA program in sync_edges
 tchecker::STATE_INTVARS_STATEMENT_FAILED if statements of the edges in sync_edges cannot be
 applied to intval
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if the updated intval does not
 satisfy the invariant of updated vloc.
 \throw std::invalid_argument : if the pid of a process involved in sync_edges is greater or equal to the
 size of vloc
 \throw std::runtime_error : if evaluation of GTA program, guards or statements
 throws an exception
 */
tchecker::state_status_t next(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::intval_sptr_t const & intval, tchecker::gtazg::gta_zone_sptr_t const & zone,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::gtazg::standard_semantics_t & semantics,
                              tchecker::gtazg::outgoing_edges_value_t const & sync_edges);

/*!
 \brief Compute next state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param sync_edges : tuple of edge from s
 \post s have been updated from sync_edges according to semantics, and
 t is the set of edges and synchronization identifier in sync_edges
 \return status of state s after update (see tchecker::gtazg::next)
 \throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t next(tchecker::gta::system_t const & system, tchecker::gtazg::state_t & s,
                                     tchecker::gtazg::transition_t & t, tchecker::gtazg::standard_semantics_t & semantics,
                                     tchecker::gtazg::outgoing_edges_value_t const & sync_edges)
{
  return tchecker::gtazg::next(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.sync_id(), semantics,
                               sync_edges);
}

// Inspector

/*!
 \brief Computes the set of labels of a state
 \param system : a system
 \param s : a state
 \return the set of labels on state s
*/
boost::dynamic_bitset<> labels(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s);

/*!
 \brief Checks is a state is a valid final state
 \param system : a system
 \param s : a state
 \return true if all prophecy clocks in zone are set to -inf, false otherwise
*/
bool is_valid_final(tchecker::gta::system_t const & system, tchecker::gtazg::gta_zone_t const & zone);

/*!
 \brief Checks is a state is a valid final state
 \param system : a system
 \param s : a state
 \return true s is a final state of system
*/
bool is_valid_final(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s);

/*!
 \brief Checks if a state is initial
 \param system : a system
 \param s : a state
 \pre s is a state computed from system
 \return true if s is an initial state in system, false otherwise
*/
bool is_initial(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s);

/*!
 \brief Computes the set of clocks released in a transition
 \param system : a system
 \param t : a vedge
 \return : the set of prophecy clocks released by t
*/
boost::dynamic_bitset<> released_clks(tchecker::gta::system_t const & system, tchecker::vedge_t const & t);

/*!
 \brief Computes the set of prophecy clocks which can be -inf in a zone
 \param system : a system
 \param zone : a gta zone
 \return : the set of prophecy clocks which can be -inf in zone
*/
boost::dynamic_bitset<> minus_inf_clks(tchecker::gta::system_t const & system, tchecker::gtazg::gta_zone_t const & zone);

// Attributes

/*!
 \brief Accessor to state attributes as strings
 \param system : a system
 \param s : a state
 \param m : a map of string pairs (key, value)
 \post the tuple of locations, the integer variables valuation and the zone in
 s have been added to map m
 */
void attributes(tchecker::gta::system_t const & system, tchecker::gtazg::state_t const & s,
                std::map<std::string, std::string> & m);

/*!
 \brief Accessor to transition attributes as strings
 \param system : a system
 \param t : a transition
 \param m : a map of string pairs (key, value)
 \post the tuple of edges in t has been added to map m
 */
void attributes(tchecker::gta::system_t const & system, tchecker::gtazg::transition_t const & t,
                std::map<std::string, std::string> & m);

// zg_t

/*!
 \class gtazg_t
 \brief Transition system of the zone graph over system of generalized timed processes with
 state and transition allocation
 \note all returned states and transitions are deallocated automatically
 */
class gtazg_t final
    : public tchecker::ts::fwd_t<tchecker::gtazg::state_sptr_t, tchecker::gtazg::const_state_sptr_t,
                                 tchecker::gtazg::transition_sptr_t, tchecker::gtazg::const_transition_sptr_t>,
      public tchecker::ts::fwd_impl_t<tchecker::gtazg::state_sptr_t, tchecker::gtazg::const_state_sptr_t,
                                      tchecker::gtazg::transition_sptr_t, tchecker::gtazg::const_transition_sptr_t,
                                      tchecker::gtazg::initial_range_t, tchecker::gtazg::outgoing_edges_range_t,
                                      tchecker::gtazg::initial_value_t, tchecker::gtazg::outgoing_edges_value_t>,
      public tchecker::ts::inspector_t<tchecker::gtazg::const_state_sptr_t, tchecker::gtazg::const_transition_sptr_t>,
      public tchecker::ts::sharing_t<tchecker::gtazg::state_sptr_t, tchecker::gtazg::transition_sptr_t> {

public:
  // Inherited types
  using fwd_t = tchecker::ts::fwd_t<tchecker::gtazg::state_sptr_t, tchecker::gtazg::const_state_sptr_t,
                                    tchecker::gtazg::transition_sptr_t, tchecker::gtazg::const_transition_sptr_t>;
  using fwd_impl_t = tchecker::ts::fwd_impl_t<tchecker::gtazg::state_sptr_t, tchecker::gtazg::const_state_sptr_t,
                                              tchecker::gtazg::transition_sptr_t, tchecker::gtazg::const_transition_sptr_t,
                                              tchecker::gtazg::initial_range_t, tchecker::gtazg::outgoing_edges_range_t,
                                              tchecker::gtazg::initial_value_t, tchecker::gtazg::outgoing_edges_value_t>;
  using inspector_t = tchecker::ts::inspector_t<tchecker::gtazg::const_state_sptr_t, tchecker::gtazg::const_transition_sptr_t>;
  using sharing_t = tchecker::ts::sharing_t<tchecker::gtazg::state_sptr_t, tchecker::gtazg::transition_sptr_t>;

  using sst_t = fwd_t::sst_t;
  using state_t = fwd_t::state_t;
  using const_state_t = fwd_t::const_state_t;
  using transition_t = fwd_t::transition_t;
  using const_transition_t = fwd_t::const_transition_t;
  using initial_range_t = fwd_impl_t::initial_range_t;
  using initial_value_t = fwd_impl_t::initial_value_t;
  using outgoing_edges_range_t = fwd_impl_t::outgoing_edges_range_t;
  using outgoing_edges_value_t = fwd_impl_t::outgoing_edges_value_t;

  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param sharing_type : type of state/transition sharing
   \param semantics : a zone semantics
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated automatically
   */
  gtazg_t(std::shared_ptr<tchecker::gta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
          std::shared_ptr<tchecker::gtazg::standard_semantics_t> const & semantics, std::size_t block_size,
          std::size_t table_size);

  /*!
   \brief Copy constructor (deleted)
   */
  gtazg_t(tchecker::gtazg::gtazg_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  gtazg_t(tchecker::gtazg::gtazg_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~gtazg_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::gtazg::gtazg_t & operator=(tchecker::gtazg::gtazg_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::gtazg::gtazg_t & operator=(tchecker::gtazg::gtazg_t &&) = delete;

  using fwd_t::state;
  using fwd_t::status;
  using fwd_t::transition;

  // Forward

  /*!
   \brief Accessor
   \return range of initial edges
   */
  virtual initial_range_t initial_edges();

  /*!
   \brief Initial state and transition from an initial edge
   \param init_edge : initial state valuation
   \param v : container
   \param mask : mask on next states
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t from init_edge, such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void initial(initial_value_t const & init_edge, std::vector<sst_t> & v,
                       tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Initial state and transition with selected status
   \param init_edge : initial state valuation
   \param v : container
   \param mask : mask on next states
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t from init_edge, such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void initial(std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual outgoing_edges_range_t outgoing_edges(tchecker::gtazg::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \param mask : mask on next states
   \post triples (status, s', t') have been added to v, for each transition s -t'-> s' along outgoing
   edge out_edge and GTA program of out_edges such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void next(tchecker::gtazg::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge,
                    std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  */
  virtual void next(tchecker::gtazg::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::gtazg::const_state_sptr_t const & s) const;

  /*!
   \brief Computes the set of prophecy clocks released by a transition
   \param t : a transition
   \return the set of prophecy clocks released by the gta program on t
   */
  virtual boost::dynamic_bitset<> released_clks(tchecker::vedge_t const & t) const;

  /*!
   \brief Computes the set of prophecy clocks which can be -inf in a state
   \param system : a system
   \param s : a state
   \return : the set of prophecy clocks which can be -inf in zone of s
  */
  virtual boost::dynamic_bitset<> minus_inf_clks(tchecker::gtazg::const_state_sptr_t const & s) const;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(tchecker::gtazg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(tchecker::gtazg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if s has a non-empty zone, false otherwise
  */
  virtual bool is_valid_final(tchecker::gtazg::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(tchecker::gtazg::const_state_sptr_t const & s) const;

  // Sharing

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::gtazg::state_sptr_t & s);

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::gtazg::transition_sptr_t & t);

  /*!
   \brief Accessor
   \return Pointer to underlying system of timed processes
  */
  inline std::shared_ptr<tchecker::gta::system_t const> system_ptr() const { return _system; }

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  inline tchecker::gta::system_t const & system() const { return *_system; }

  /*!
   \brief Accessor
   \return sharing type of this synchronized product
  */
  inline enum tchecker::ts::sharing_type_t sharing_type() const { return _sharing_type; }

  /*!
   \brief Accessor
   \return Indices of prophecy clks in DBM
  */
  inline tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx() const { return _prophecy_clks_idx; }

  /*!
   \brief Accessor
   \return Indices of history clks in DBM
  */
  inline tchecker::gtadbm::history_clks_idx_t const & history_clks_idx() const { return _history_clks_idx; }

private:
  std::shared_ptr<tchecker::gta::system_t const> _system; /*!< System of timed processes */

  tchecker::gtadbm::prophecy_clks_idx_t const _prophecy_clks_idx; /*< Indices of prophecy clks in the DBM*/
  tchecker::gtadbm::history_clks_idx_t const _history_clks_idx;   /*< Indices of history clks in the DBM*/

  enum tchecker::ts::sharing_type_t _sharing_type;                    /*!< Sharing of state/transition components */
  std::shared_ptr<tchecker::gtazg::standard_semantics_t> _semantics;  /*!< Zone semantics */
  tchecker::gtazg::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::gtazg::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

/*!
 \brief Factory of zone graphs
 \param system : system of generalized timed processes
 \param sharing_type : type of sharing
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with standard zone semantics and allocation of block_size objects at a time
 */
tchecker::gtazg::gtazg_t * factory(std::shared_ptr<tchecker::gta::system_t const> const & system,
                                   enum tchecker::ts::sharing_type_t sharing_type, std::size_t block_size,
                                   std::size_t table_size);

} // end of namespace gtazg

} // end of namespace tchecker

#endif // TCHECKER_GTAZG_HH
