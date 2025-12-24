/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_HH
#define TCHECKER_GTA_HH

#include <cstdlib>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/gta/allocators.hh"
#include "tchecker/gta/state.hh"
#include "tchecker/gta/system.hh"
#include "tchecker/gta/transition.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/ts/fwd.hh"
#include "tchecker/ts/inspector.hh"
#include "tchecker/ts/sharing.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file gta.hh
 \brief Generalized Timed automata
 */

namespace tchecker {

namespace gta {

// Initial edges

/*!
 \brief Type of iterator over initial states
 */
using initial_iterator_t = tchecker::ta::initial_iterator_t;

/*!
\brief Type of range of iterators over inital states
*/
using initial_range_t = tchecker::ta::initial_range_t;

/*!
 \brief Accessor to initial edges
 \param system : a gta system
 \return initial edges
 */
inline tchecker::gta::initial_range_t initial_edges(tchecker::gta::system_t const & system)
{
  return tchecker::ta::initial_edges(system.as_ta_system());
}

/*!
 \brief Dereference type for iterator over initial states
 */
using initial_value_t = tchecker::ta::initial_value_t;

// Initial state

/*!
 \brief Compute initial state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param initial_range : range of initial state valuations
 \pre the size of vloc and vedge is equal to the size of initial_range.
 initial_range has been obtained from system.
 initial_range yields the initial locations of all the processes ordered by increasing process identifier
 \post vloc has been initialized to the tuple of initial locations in initial_range,
 intval has been initialized to the initial valuation of bounded integer variables,
 vedge has been initialized to an empty tuple of edges.
 sync_id has been set to tchecker::NO_SYNC.
 \return tchecker::STATE_OK if initialization succeeded
 STATE_SRC_INVARIANT_VIOLATED if the initial valuation of integer variables does not satisfy invariant
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t initial(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                 tchecker::intval_sptr_t const & intval, tchecker::vedge_sptr_t const & vedge,
                                 tchecker::sync_id_t & sync_id, tchecker::gta::initial_value_t const & initial_range);

/*!
\brief Compute initial state and transition
\param system : a system
\param s : state
\param t : transition
\param v : initial iterator value
\post s has been initialized from v (and has empty vedge, no sync identifier, empty guard and empty source invariant)
\return tchecker::STATE_OK
\throw std::invalid_argument : if s and v have incompatible sizes
*/
inline tchecker::state_status_t initial(tchecker::gta::system_t const & system, tchecker::gta::state_t & s,
                                        tchecker::gta::transition_t & t, tchecker::gta::initial_value_t const & v)
{
  return tchecker::gta::initial(system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.sync_id(), v);
}

// Outgoing edges

/*!
 \brief Type of iterator over outgoing edges
 */
using outgoing_edges_iterator_t = tchecker::ta::outgoing_edges_iterator_t;

/*!
\brief Type of range of outgoing edges
*/
using outgoing_edges_range_t = tchecker::ta::outgoing_edges_range_t;

/*!
 \brief Accessor to outgoing edges
 \param system : a gta system
 \param vloc : tuple of locations
 \return range of outgoing asynchronous edges from vloc in system
 */
inline tchecker::gta::outgoing_edges_range_t outgoing_edges(tchecker::gta::system_t const & system,
                                                            tchecker::const_vloc_sptr_t const & vloc)
{
  return tchecker::ta::outgoing_edges(system.as_ta_system(), vloc);
}

/*!
 \brief Type of outgoing vedge (range of synchronized/asynchronous edges)
 */
using outgoing_edges_value_t = tchecker::ta::outgoing_edges_value_t;

// Next state

/*!
 \brief Compute next state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param sync_edges : synchronized edges from vloc
 \pre the source location in the edges in sync_edges match the locations in vloc.
 No process has more than one edge in sync_edges.
 The pid of every process in sync_edges is less than the size of vloc
 \post the locations in vloc have been updated to target locations of the
 processes involved in sync_edges, and they have been left unchanged for the other processes.
 The values of variables in intval have been updated according to the statements in sync_edges.
 vedge contains the identifiers of the edges in sync_edges
 vedge is an instance of synchronization sync_id, unless sync_id is tchecker::NO_SYNC (asynchronous vedge)
 \return STATE_OK if state computation succeeded,
 STATE_INCOMPATIBLE_EDGE if the source locations of the edges in sync_edges do not match vloc,
 STATE_SRC_INVARIANT_VIOLATED if the valuation intval does not satisfy the invariant in vloc,
 STATE_GUARD_VIOLATED if the values in intval do not satisfy the guard of the edges in sync_edges,
 STATE_STATEMENT_FAILED if statements of the edges in sync_edges cannot be applied to intval
 STATE_TGT_INVARIANT_VIOLATED if the updated intval does not satisfy the invariant of updated vloc.
 \throw std::invalid_argument : if a pid of the processes involved in sync_edges is greater or equal to the size of vloc
 \throw std::runtime_error : if the guards of the edges in sync_edges generate clock resets, or if the statements of the edges
 in sync_edges generate clock constraints, or if the invariant in updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements throws an exception
 */
tchecker::state_status_t next(tchecker::gta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::intval_sptr_t const & intval, tchecker::vedge_sptr_t const & vedge,
                              tchecker::sync_id_t & sync_id, tchecker::gta::outgoing_edges_value_t const & sync_edges);

/*!
 \brief Compute next state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param sync_edges : synchronized edges from s
 \post s have been updated from sync_edges, and t is the tuple of edges and synchronization identifier in sync_edges and the
 synchronization identifier in sync_edges
 \return status of state s after update
 \throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t next(tchecker::gta::system_t const & system, tchecker::gta::state_t & s,
                                     tchecker::gta::transition_t & t, tchecker::gta::outgoing_edges_value_t const & sync_edges)
{
  return tchecker::gta::next(system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.sync_id(), sync_edges);
}

// Tools

/*!
 \brief Checks if time can elapse in a tuple of locations
 \param system : a system of generalized timed processes
 \param vloc : tuple of locations
 \return true if time delay is allowed in vloc, false otherwise
 */
inline bool delay_allowed(tchecker::gta::system_t const & system, tchecker::vloc_t const & vloc)
{
  return tchecker::ta::delay_allowed(system.as_ta_system(), vloc);
}

// Inspector

/*!
  \brief Computes the set of labels of a state
  \param system : a system
  \param s : a state
  \return the set of labels on state s
  */
inline boost::dynamic_bitset<> labels(tchecker::gta::system_t const & system, tchecker::gta::state_t const & s)
{
  return tchecker::ta::labels(system.as_ta_system(), s);
}

/*!
 \brief Checks if a state is a valid final state
 \param system : a system
 \param s : a state
 \return true
*/
inline bool is_valid_final(tchecker::gta::system_t const & system, tchecker::gta::state_t const & s)
{
  return tchecker::ta::is_valid_final(system.as_ta_system(), s);
}

/*!
 \brief Checks if a bounded integer variables valuations is initial
 \param system : a system
 \param v : a valuation
 \pre the size of v corresponds to the number of flattened integer variables in system (check
 by assertion)
 \return true if v is an initial valuation of the bounded integer variables in system,
 false otherwise
*/
inline bool is_initial(tchecker::gta::system_t const & system, tchecker::intval_t const & v)
{
  return tchecker::ta::is_initial(system.as_ta_system(), v);
}

/*!
 \brief Checks if a state is initial
 \param system : a system
 \param s : a state
 \pre s is a state computed from system
 \return true if s is an initial state in system, false otherwise
*/
inline bool is_initial(tchecker::gta::system_t const & system, tchecker::gta::state_t const & s)
{
  return tchecker::ta::is_initial(system.as_ta_system(), s);
}

// Attributes

/*!
 \brief Accessor to state attributes as strings
 \param system : a system
 \param s : a state
 \param m : a map of string pairs (key, value)
 \post the tuple of locations and integer variables valuation in s have been
 added to map m
 */
inline void attributes(tchecker::gta::system_t const & system, tchecker::gta::state_t const & s,
                       std::map<std::string, std::string> & m)
{
  tchecker::ta::attributes(system.as_ta_system(), s, m);
}

/*!
 \brief Accessor to transition attributes as strings
 \param system : a system
 \param t : a transition
 \param m : a map of string pairs (key, value)
 \post the tuple of edges in t has been added to map m
 */
inline void attributes(tchecker::gta::system_t const & system, tchecker::gta::transition_t const & t,
                       std::map<std::string, std::string> & m)
{
  tchecker::ta::attributes(system.as_ta_system(), t, m);
}

// gta_t

/*!
 \class gta_t
 \brief Transition system of the generalized timed automaton over processes of timed processes with
 state and transition allocation
 \note all returned states and transitions are deallocated automatically
 */
class gta_t final : public tchecker::ts::fwd_t<tchecker::gta::state_sptr_t, tchecker::gta::const_state_sptr_t,
                                               tchecker::gta::transition_sptr_t, tchecker::gta::const_transition_sptr_t>,
                    public tchecker::ts::fwd_impl_t<tchecker::gta::state_sptr_t, tchecker::gta::const_state_sptr_t,
                                                    tchecker::gta::transition_sptr_t, tchecker::gta::const_transition_sptr_t,
                                                    tchecker::gta::initial_range_t, tchecker::gta::outgoing_edges_range_t,
                                                    tchecker::gta::initial_value_t, tchecker::gta::outgoing_edges_value_t>,
                    public tchecker::ts::inspector_t<tchecker::gta::const_state_sptr_t, tchecker::gta::const_transition_sptr_t>,
                    public tchecker::ts::sharing_t<tchecker::gta::state_sptr_t, tchecker::gta::transition_sptr_t> {
public:
  // Inherited types
  using fwd_t = tchecker::ts::fwd_t<tchecker::gta::state_sptr_t, tchecker::gta::const_state_sptr_t,
                                    tchecker::gta::transition_sptr_t, tchecker::gta::const_transition_sptr_t>;
  using fwd_impl_t = tchecker::ts::fwd_impl_t<tchecker::gta::state_sptr_t, tchecker::gta::const_state_sptr_t,
                                              tchecker::gta::transition_sptr_t, tchecker::gta::const_transition_sptr_t,
                                              tchecker::gta::initial_range_t, tchecker::gta::outgoing_edges_range_t,
                                              tchecker::gta::initial_value_t, tchecker::gta::outgoing_edges_value_t>;
  using inspector_t = tchecker::ts::inspector_t<tchecker::gta::const_state_sptr_t, tchecker::gta::const_transition_sptr_t>;
  using sharing_t = tchecker::ts::sharing_t<tchecker::gta::state_sptr_t, tchecker::gta::transition_sptr_t>;

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
   \param system : a system of generalized timed processes
   \param sharing_type : type of state/transition sharing
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables\note all states and transitions are pool allocated and deallocated automatically.
   Components within states and transitions are shared if sharing_type is tchecker::ts::SHARING
   */
  gta_t(std::shared_ptr<tchecker::gta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
        std::size_t block_size, std::size_t table_size);

  /*!
   \brief Copy constructor (deleted)
   */
  gta_t(tchecker::gta::gta_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  gta_t(tchecker::gta::gta_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~gta_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::gta::gta_t & operator=(tchecker::gta::gta_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::gta::gta_t & operator=(tchecker::gta::gta_t &&) = delete;

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
   \param init_edge : initial edge valuation
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
  virtual outgoing_edges_range_t outgoing_edges(tchecker::gta::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition from an outgoing edge
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \param mask : mask on next states
   \post triples (status, s', t') have been added to v, for each transition s -t'-> s' along outgoing
   edge out_edge such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void next(tchecker::gta::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge,
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
  virtual void next(tchecker::gta::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::gta::const_state_sptr_t const & s) const;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(tchecker::gta::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(tchecker::gta::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if a run ending in s is a valid run, false otherwise
  */
  virtual bool is_valid_final(tchecker::gta::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(tchecker::gta::const_state_sptr_t const & s) const;

  // Sharing

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::gta::state_sptr_t & s);

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::gta::transition_sptr_t & t);

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

private:
  std::shared_ptr<tchecker::gta::system_t const> _system;           /*!< System of timed processes */
  enum tchecker::ts::sharing_type_t _sharing_type;                  /*!< Sharing of state/transition components */
  tchecker::gta::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::gta::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

} // namespace gta

} // end of namespace tchecker

#endif // TCHECKER_GTA_HH
