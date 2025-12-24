/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTAZG_STATE_HH
#define TCHECKER_GTAZG_STATE_HH

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/gta/state.hh"
#include "tchecker/gtazg/gta_zone.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file state.hh
 \brief State of a gta zone graph
 */

namespace tchecker {

namespace gtazg {

/*!
\brief Type of shared gta zone, DBM implementation
*/
using shared_gta_zone_t = tchecker::make_shared_t<tchecker::gtazg::gta_zone_t>;

/*!
\brief Type of shared pointer to gta zone, DBM implementation
*/
using gta_zone_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t>;

/*!
 \class state_t
 \brief state of a gta zone graph
 */
class state_t : public tchecker::gta::state_t {
public:
  /*!
   \brief Constructor
   \param vloc : tuple of locations
   \param intval : integer variables valuation
   \param zone : gta zone of clock valuations
   \pre vloc, intval and zone must not point to nullptr (checked by assertion)
   */
  explicit state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                   tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                   tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t> const & zone);

  /*!
   \brief Partial copy constructor
   \param state : a state
   \param vloc : tuple of locations
   \param intval : integer variables valuation
   \param zone : gta zone of clock valuations
   \pre vloc, intval and zone must not point to nullptr (checked by assertion)
   \note the state is copied from s, except the tuple of locations which is
   initialized from vloc, and the valuation of bounded integer variables which
   is initialized from intval, and the gta zone which is initialized from zone
   */
  state_t(tchecker::gta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
          tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
          tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t> const & zone);

  /*!
   \brief Copy constructor (deleted)
   */
  state_t(tchecker::gtazg::state_t const &) = delete;

  /*!
   \brief Move constructor
   */
  state_t(tchecker::gtazg::state_t &&) = default;

  /*!
   \brief Destructor
   */
  ~state_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::gtazg::state_t & operator=(tchecker::gtazg::state_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::gtazg::state_t & operator=(tchecker::gtazg::state_t &&) = default;

  /*!
  \brief Accessor
  \return zone in this state
  */
  constexpr inline tchecker::gtazg::gta_zone_t const & zone() const { return *_zone; }

  /*!
  \brief Accessor
  \return reference to pointer to the gta zone in this state
  */
  inline tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t> & zone_ptr() { return _zone; }

  /*!
  \brief Accessor
  \return pointer to const gta zone in this state
  */
  inline tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t const> zone_ptr() const
  {
    return tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t const>(_zone.ptr());
  }

private:
  tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_gta_zone_t> _zone; /*!< GTA Zone over clock valuations */
};

/*!
 \brief Equality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same zone, same tuple of locations and same
 valuation of bounded integer variables, false otherwise
 */
bool operator==(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2);

/*!
 \brief Disequality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have different zones, or different tuples of locations or different valuations of bounded integer
 variables, false otherwise
 */
bool operator!=(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2);

/*!
 \brief Equality check for shared states
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same zone, same tuple of locations and same
 valuation of bounded integer variables, false otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointer equality (not values equality)
 */
bool shared_equal_to(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2);

/*!
 \brief Checks inclusion wrt gsim clockbounds
 \param s1 : gta zg state
 \param s2 : gta zg state
 \param vloc_to_bounds : Computes bounds from a vloc
 \return true if s1 and s2 have same locations and intvals, and s1.zone <=G s2.zone false otherwise
 */
bool is_gmap_le(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2,
                tchecker::clockbounds::gta::vloc_to_bounds_t const & vloc_to_bounds);

/*!
 \brief Checks inclusion wrt gsim clockbounds for shared states
 \param s1 : gta zg state
 \param s2 : gta zg state
 \param vloc_to_bounds : Computes bounds from a vloc
 \return true if s1 and s2 have same locations and intvals, and s1.zone <=G s2.zone false otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointers instead of values when relevant
 */
bool shared_is_gmap_le(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2,
                       tchecker::clockbounds::gta::vloc_to_bounds_t const & vloc_to_bounds);

/*!
 \brief Hash
 \param s : state
 \return Hash value for state s
 */
std::size_t hash_value(tchecker::gtazg::state_t const & s);

/*!
 \brief Hash for shared states
 \param s : state
 \return Hash value for state s
 \note this should only be used on states that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::gtazg::state_t const & s);

/*!
 \brief Parital Hash for shared states
 \param s : state
 \return Hash value for discrete state of s and lower bounds on prophecy clocks
 \note this should only be used on states that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_partial_hash_value(tchecker::gtazg::state_t const & s);

/*!
 \brief Lexical ordering on states of the zone graph
 \param s1 : a state
 \param s2 : a state
 \return 0 if s1 and s2 are equal, a negative value if s1 is smaller than s2
 w.r.t. lexical ordering on zone,tuple of locations and valuation of bounded
 integer variables, a positive value otherwise
 */
int lexical_cmp(tchecker::gtazg::state_t const & s1, tchecker::gtazg::state_t const & s2);

/*!
 \brief Type of shared state
 */
using shared_state_t = tchecker::make_shared_t<tchecker::gtazg::state_t>;

/*!
\brief Type of pointer to shared state
*/
using state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_state_t>;

/*!
\brief Type of pointer to shared const state
*/
using const_state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::gtazg::shared_state_t const>;

} // namespace gtazg

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::gtazg::state_t
 */
template <> class allocation_size_t<tchecker::gtazg::state_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::zg::state_t
   \return allocation size for objects of class tchecker::zg::state_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args) { return sizeof(tchecker::gtazg::state_t); }
};

} // end of namespace tchecker

#endif // TCHECKER_GTAZG_STATE_HH
