/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_TRANSITION_HH
#define TCHECKER_GTA_TRANSITION_HH

#include "tchecker/ta/transition.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file transition.hh
 \brief Transitions for generalized timed automata
 */

namespace tchecker {

namespace gta {

/*!
 \class transition_t
 \brief Transition for generalized timed automata
 */
class transition_t : public tchecker::ta::transition_t {
public:
  /*!
   \brief Constructor
   \param sync_id synchronization identifier
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \pre sync_id is tchecker::NO_SYNC, since we do not allow for synchronized edges in GTA(checked by assertion)
   */
  explicit transition_t(tchecker::sync_id_t sync_id, tchecker::vedge_sptr_t const & vedge);

  /*!
   \brief Partial copy constructor
   \param t : a transition
   \param sync_id : synchronization identifier
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \pre sync_id is tchecker::NO_SYNC, since we do not allow for synchronized edges in GTA(checked by assertion)
   \note the transition is copied from t, except the synchronization identifier which is set to sync_id,
   and the tuple of edges which is initialized from vedge
   */
  transition_t(tchecker::gta::transition_t const & t, tchecker::sync_id_t sync_id, tchecker::vedge_sptr_t const & vedge);

  /*!
   \brief Copy constructor (deleted)
   */
  transition_t(tchecker::gta::transition_t const &) = delete;

  /*!
   \brief Move constructor
   */
  transition_t(tchecker::gta::transition_t &&) = default;

  /*!
   \brief Destructor
   */
  ~transition_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::gta::transition_t & operator=(tchecker::gta::transition_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::gta::transition_t & operator=(tchecker::gta::transition_t &&) = default;

  // Container accessors (deleted since clock constraints are only present in GTA programs)

  tchecker::clock_constraint_container_t & src_invariant_container() = delete;
  tchecker::clock_constraint_container_t const & src_invariant_container() const = delete;
  tchecker::clock_constraint_container_t & guard_container() = delete;
  tchecker::clock_constraint_container_t const & guard_container() const = delete;
  tchecker::clock_reset_container_t & reset_container() = delete;
  tchecker::clock_reset_container_t const & reset_container() const = delete;
  tchecker::clock_constraint_container_t & tgt_invariant_container() = delete;
  tchecker::clock_constraint_container_t const & tgt_invariant_container() const = delete;

  // Range accessors (deleted since clock constraints are only present in GTA programs)
  tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> src_invariant() const = delete;
  tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> guard() const = delete;
  tchecker::range_t<tchecker::clock_reset_container_const_iterator_t> reset() const = delete;
  tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> tgt_invariant() const = delete;
};

/*!
 \brief Equality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same synchronization identifier and the same tuple of edges
 */
bool operator==(tchecker::gta::transition_t const & t1, tchecker::gta::transition_t const & t2);

/*!
 \brief Disequality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have different synchronization identifiers, or different tuples of edges
 */
inline bool operator!=(tchecker::gta::transition_t const & t1, tchecker::gta::transition_t const & t2) { return !(t1 == t2); }

/*!
 \brief Equality check for shared transitions
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same synchronization identifier, and poinr to the same tuple of edges
 \note note this should only be used on transitions that have shared internal components: this function checks pointer equality
 (not values equality)
 */
bool shared_equal_to(tchecker::gta::transition_t const & t1, tchecker::gta::transition_t const & t2);

/*!
 \brief Hash
 \param t : transition
 \return Hash value for transition t
 */
std::size_t hash_value(tchecker::gta::transition_t const & t);

/*!
 \brief Hash for shared transitions
 \param t : transition
 \return Hash value for transition t
 \note note this should only be used on transitions that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::gta::transition_t const & t);

/*!
 \brief Lexical ordering on transitions of generalized timed automata
 \param t1 : first transition
 \param t2 : second transition
 \return 0 if t1 and t2 are equal, a negative value if t1 is smaller than t2 w.r.t. lexical ordering on tuple of edges and
 positive value otherwise
 */
int lexical_cmp(tchecker::gta::transition_t const & t1, tchecker::gta::transition_t const & t2);

/*!
 \brief Type of shared transition
 */
using shared_transition_t = tchecker::make_shared_t<tchecker::gta::transition_t>;

/*!
\brief Type of pointer to shared transition
*/
using transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gta::shared_transition_t>;

/*!
\brief Type of pointer to shared const transition
*/
using const_transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gta::shared_transition_t const>;

} // end of namespace gta

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::ta::transition_t
 */
template <> class allocation_size_t<tchecker::gta::transition_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::ta::transition_t
   \return allocation size for objects of class tchecker::ta::transition_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... /*args*/)
  {
    return sizeof(tchecker::gta::transition_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_GTA_TRANSITION_HH
