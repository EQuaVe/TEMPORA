/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_STATE_HH
#define TCHECKER_GTA_STATE_HH

#include "tchecker/ta/state.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file state.hh
 \brief States for generalized timed automata
 */

namespace tchecker {

namespace gta {

/*!
 \class state_t
 \brief state for generalized timed automata
 */
class state_t : public tchecker::ta::state_t {
public:
  /*!
   \brief Constructor
   \param vloc : tuple of locations
   \param intval : bounded integer variables valuation
   \pre vloc and intval must not point to nullptr (checked by assertion)
   */
  explicit state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                   tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval);

  /*!
   \brief Partial copy constructor
   \param state : a state
   \param vloc : tuple of locations
   \param intval : bounded integer variables valuation
   \pre vloc and intval must not point to nullptr (checked by assertion)
   \note the state is copied from s, except the tuple of locations which is initialized from vloc, and the valuation of bounded
   integer variables which is initialized from intval
   */
  state_t(tchecker::gta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
          tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval);

  /*!
   \brief Copy constructor (deleted)
   */
  state_t(tchecker::gta::state_t const &) = delete;

  /*!
   \brief Move constructor
   */
  state_t(tchecker::gta::state_t &&) = default;

  /*!
   \brief Destructor
   */
  ~state_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::gta::state_t & operator=(tchecker::gta::state_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::gta::state_t & operator=(tchecker::gta::state_t &&) = default;

  using tchecker::ta::state_t::intval;
  using tchecker::ta::state_t::intval_ptr;

private:
};

/*!
 \brief Equality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same tuple of locations and same valuation of bounded integer variables, false otherwise
 */
bool operator==(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2);

/*!
 \brief Disequality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have different tuples of locations or different valuations of bounded integer variables, false
 otherwise
 */
bool operator!=(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2);

/*!
 \brief Equality check for shared states
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same tuple of locations and same valuation of
 bounded integer variables, false otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointer equality (not values equality)
 */
bool shared_equal_to(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2);

/*!
 \brief Hash
 \param s : state
 \return Hash value for state s
 */
std::size_t hash_value(tchecker::gta::state_t const & s);

/*!
 \brief Hash for shared states
 \param s : state
 \return Hash value for state s
 \note this should be used on states that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::gta::state_t const & s);

/*!
 \brief Lexical ordering on states of generalized timed automata
 \param s1 : a state
 \param s2 : a state
 \return 0 if s1 and s2 are equal, a negative value if s1 is smaller than s2 w.r.t. lexical ordering on tuple of locations and
 valuatin of bounded integer variables, a positive value otherwise
 */
int lexical_cmp(tchecker::gta::state_t const & s1, tchecker::gta::state_t const & s2);

/*!
 \brief Type of shared state
 */
using shared_state_t = tchecker::make_shared_t<tchecker::gta::state_t>;

/*!
\brief Type of pointer to shared state
*/
using state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gta::shared_state_t>;

/*!
\brief Type of pointer to shared const state
*/
using const_state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gta::shared_state_t const>;

} // namespace gta

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::gta::state_t
 */
template <> class allocation_size_t<tchecker::gta::state_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::gta::state_t
   \return allocation size for objects of class tchecker::gta::state_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... /*args*/)
  {
    return sizeof(tchecker::gta::state_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_GTA_STATE_HH
