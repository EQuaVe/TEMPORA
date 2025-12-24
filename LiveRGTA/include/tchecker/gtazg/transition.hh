/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTAZG_TRANSITION_HH
#define TCHECKER_GTAZG_TRANSITION_HH

#include "tchecker/gta/transition.hh"
#include "tchecker/gta_program/static_analysis.hh"
#include "tchecker/utils/allocation_size.hh"

/*!
 \file transition.hh
 \brief Transitions for gta zone graphs
 */

namespace tchecker {

namespace gtazg {

/*!
 \class transition_t
 \brief Transition for generalized zone graphs
 */
class transition_t : public tchecker::gta::transition_t {
public:
  /*!
   \brief Constructor
   \param sync_id synchronization identifier
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \pre sync_id is tchecker::NO_SYNC (checked by assertion)
   */
  explicit transition_t(tchecker::sync_id_t sync_id, tchecker::vedge_sptr_t const & vedge);

  /*!
   \brief Partial copy constructor
   \param t : a transition
   \param sync_id : synchronization identifier
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \pre sync_id is tchecker::NO_SYNC (checked by assertion)
   \note the transition is copied from t, except the synchronization identifier which is set to sync_id, the tuple of edges
   which is initialized from vedge, and the compiled gta program with its index
   */
  transition_t(tchecker::gtazg::transition_t const & t, tchecker::sync_id_t sync_id, tchecker::vedge_sptr_t const & vedge);

  /*!
   \brief Copy constructor (deleted)
   */
  transition_t(tchecker::gtazg::transition_t const &) = delete;

  /*!
   \brief Move constructor
   */
  transition_t(tchecker::gtazg::transition_t &&) = default;

  /*!
   \brief Destructor
   */
  ~transition_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::gtazg::transition_t & operator=(tchecker::gtazg::transition_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::gtazg::transition_t & operator=(tchecker::gtazg::transition_t &&) = default;
};

/*!
 \brief Equality check
 \param t1 : transition
 \param t2 : transition
 \return true iff t1 and t2 as GTA transitions are equal, and have the same GTA program idx
 */
bool operator==(tchecker::gtazg::transition_t const & t1, tchecker::gtazg::transition_t const & t2);

/*!
 \brief Disequality check
 \param t1 : transition
 \param t2 : transition
 \return true if either t1 is not equal to t2 as GTA edges, or do not have the same GTA program idx
 \return see tchecker::::operator!= on transitions
 */
inline bool operator!=(tchecker::gtazg::transition_t const & t1, tchecker::gtazg::transition_t const & t2)
{
  return !(t1 == t2);
}

/*!
 \brief Equality check for shared transitions
 \param t1 : transition
 \param t2 : transition
 \return true if either t1 is not equal to t2 as GTA edges, or do not have the same GTA program idx
 \note note this should only be used on transitions that have shared internal components: this
 function checks pointer equality (not values equality)
 */
bool shared_equal_to(tchecker::gtazg::transition_t const & t1, tchecker::gtazg::transition_t const & t2);

/*!
 \brief Hash
 \param t : transition
 \return Hash value for transition t
 */
std::size_t hash_value(tchecker::gtazg::transition_t const & t);

/*!
 \brief Hash for shared transitions
 \param t : transition
 \return Hash value for transition t
 \note note this should only be used on transitions that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::gtazg::transition_t const & t);

/*!
 \brief Lexical ordering on transitions of the zone graph
 \param t1 : first transition
 \param t2 : second transition
 \return 0 if t1 and t2 are equal, a negative value if t1 is smaller than t2 w.r.t. lexical ordering on tuple of edges or the
 index of GTA program, and positive otherwise
 */
int lexical_cmp(tchecker::gtazg::transition_t const & t1, tchecker::gtazg::transition_t const & t2);

/*!
 \brief Type of shared transition
 */
using shared_transition_t = tchecker::make_shared_t<tchecker::gtazg::transition_t>;

/*!
\brief Type of pointer to shared transition
*/
using transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_transition_t>;

/*!
\brief Type of pointer to shared const transition
*/
using const_transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_transition_t const>;

} // namespace gtazg

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::gtazg::transition_t
 */
template <> class allocation_size_t<tchecker::gtazg::transition_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::gtazg::transition_t
   \return allocation size for objects of class tchecker::gtazg::transition_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args)
  {
    return sizeof(tchecker::gtazg::transition_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_GTAZG_TRANSITION_HH
