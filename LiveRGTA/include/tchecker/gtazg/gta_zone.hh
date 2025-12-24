/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_ZG_ZONE_HH
#define TCHECKER_GTA_ZG_ZONE_HH

#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/gtadbm.hh"
#include "tchecker/utils/cache.hh"

/*!
 \file zone.hh
 \brief DBM implementation of gta zones
 */

namespace tchecker {

namespace gtazg {

/*!
 \class gta_zone_t
 \brief DBM implementation of gta zones
 */
class gta_zone_t : public tchecker::cached_object_t {
public:
  /*!
   \brief Assignment operator
   \param zone : a DBM gta_zone
   \pre this and zone have the same dimension
   \post this is a copy of zone
   \return this after assignment
   \throw std::invalid_argument : if this and zone do not have the same dimension
   */
  tchecker::gtazg::gta_zone_t & operator=(tchecker::gtazg::gta_zone_t const & zone);

  /*!
   \brief Move assignment operator
   \note deleted (same as assignment operator)
   */
  tchecker::gtazg::gta_zone_t & operator=(tchecker::gtazg::gta_zone_t && zone) = delete;

  /*!
   \brief Emptiness check
   \return true if this zone is empty, false otherwise
   */
  bool is_empty() const;

  /*!
   \brief Checks if the zone is a final zone
   \return true if all prophecy clocks can take value -inf in the zone, false otherwise
   */
  bool is_final() const;

  /*!
   \brief Equality predicate
   \param zone : a DBM gta_zone
   \return true if this is equal to zone, false otherwise
   */
  bool operator==(tchecker::gtazg::gta_zone_t const & zone) const;

  /*!
   \brief Disequality predicate
   \param zone : a DBM gta_zone
   \return true if this is not equal to zone, false otherwise
   */
  bool operator!=(tchecker::gtazg::gta_zone_t const & zone) const;

  /*!
   \brief Checks inclusion wrt gsim clockbounds
   \param zone : a GTA DBM zone
   \param bounds : gsim clockbounds corresponding to the vloc of zone1 and zone2
   \return true if this zone is included in zone w.r.t g-simulation for bounds, false otherwise
   */
  bool is_gmap_le(tchecker::gtazg::gta_zone_t const & zone, tchecker::clockbounds::gta::gsim_map_t & clockbounds) const;

  /*!
   \brief Lexical ordering
   \param zone : a DBM gta_zone
   \return 0 if this and zone are equal, a negative value if this is smaller than zone w.r.t. lexical ordering on the clock
   constraints, a positive value otherwise
   */
  int lexical_cmp(tchecker::gtazg::gta_zone_t const & zone) const;

  /*!
   \brief Accessor
   \return hash code for this zone
   */
  std::size_t hash() const;

  /*!
   \brief Accessor
   \return partial hash code for this zone
   */
  std::size_t prophecy_clks_lower_bound_hash() const;
  /*!
   \brief Accessor
   \return dimension of the zone
   */
  inline std::size_t dim() const { return _dim; }

  /*!
   \brief Accessor
   \return Indices for prophecy clocks in DBM
   */
  inline tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx() const { return _prop_clks; }

  /*!
   \brief Accessor
   \return Indices for history clocks in DBM
   */
  inline tchecker::gtadbm::history_clks_idx_t const & history_clks_idx() const { return _hist_clks; }

  /*!
   \brief Output
   \param os : output stream
   \param index : clock index (map clock ID -> clock name)
   \pre index is a clock index over system clocks (the first clock has index 0)
   \post this zone has been output to os with clock names from index
   \return os after this zone has been output
   */
  std::ostream & output(std::ostream & os, tchecker::clock_index_t const & index) const;

  /*!
   \brief Accessor
   \return internal DBM of size dim()*dim()
   \note Modifications to the returned DBM should ensure tightness and standard form or emptiness of the zone, following the
   convention defined in file tchecker/dbm/gtadbm.hh. It is thus strongly suggested to use the function defined in that file to
   modify the returned DBM. Otherwise, the methods may not be accurate over this zone.
   \note The DBM in a zone may be shared with other zones (if the zone has been enerated by a sharing zone graph). DO NOT MODIFY
   a shared DBM as it would modify the DBM in multiple zones at once
   */
  tchecker::dbm::extended::db_t * dbm();

  /*!
   \brief Accessor
   \return internal DBM of size dim()*dim()
   */
  tchecker::dbm::extended::db_t const * dbm() const;

  /*!
  \brief Conversion to DBM
  \param dbm : a DBM
  \pre dbm is a dim() * dim() allocated DBM
  \post dbm contains a DBM representation of the zone.
  dbm is tight if the zone is not empty.
   */
  void to_dbm(tchecker::dbm::extended::db_t * dbm) const;

  /*!
   \brief Construction
   \tparam ARGS : type of arguments to a constructor of tchecker::gtazg::gta_zone_t
   \tparam ptr : pointer to an allocated gta_zone
   \pre ptr points to an allocated zone of sufficient capacity, i.e. at least
   allocation_size_t<tchecker::gtazg::gta_zone_t>::alloc_size(dim)
   \post an instance of tchecker::gtazg::gta_zone_t has been built in ptr with
   parameters args
   */
  template <class... ARGS> static inline void construct(void * ptr, ARGS &&... args)
  {
    new (ptr) tchecker::gtazg::gta_zone_t(args...);
  }

  /*!
   \brief Destruction
   \param ptr : a gta_zone
   \post the destructor of tchecker::gtazg::gta_zone_t has been called on ptr
   */
  static inline void destruct(tchecker::gtazg::gta_zone_t * ptr) { ptr->~gta_zone_t(); }

protected:
  /*!
   \brief Constructor
   \param dim : dimension
   \param prophecy_clks_idx : Indices of prophecy clks
   \param history_clks_idx : Indices of history clks
   \post this zone has dimension dim and is the universal zone
   */
  gta_zone_t(tchecker::clock_id_t dim, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
             tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

  /*!
   \brief Copy constructor
   \param zone : a gta_zone
   \pre this has been allocated with the same dimension as zone
   \post this is a copy of zone
   */
  gta_zone_t(tchecker::gtazg::gta_zone_t const & zone);

  /*!
   \brief Move constructor
   \note deleted (move construction is the same as copy construction)
   */
  gta_zone_t(tchecker::gtazg::gta_zone_t && zone) = delete;

  /*!
   \brief Destructor
   */
  ~gta_zone_t() = default;

  /*!
   \brief Accessor
   \return pointer to DBM
   */
  constexpr tchecker::dbm::extended::db_t * dbm_ptr() const
  {
    return static_cast<tchecker::dbm::extended::db_t *>(
        static_cast<void *>(const_cast<tchecker::gtazg::gta_zone_t *>(this) + 1));
  }

  /*!
   \brief Accessor
   \param i : clock index
   \param j : clock index
   \return constraint on xi-xj in this DBM
   */
  constexpr tchecker::dbm::extended::db_t dbm(tchecker::gtadbm::clk_idx_t const i, tchecker::gtadbm::clk_idx_t const j) const
  {
    return dbm_ptr()[i * _dim + j];
  }

  tchecker::clock_id_t _dim;                                /*!< Dimension of DBM */
  tchecker::gtadbm::prophecy_clks_idx_t const & _prop_clks; /*!< Indices of prophecy clks */
  tchecker::gtadbm::history_clks_idx_t const & _hist_clks;  /*!< Indices of history clks */
};

/*!
 \brief Boost compatible hash function on gta zones
 \param zone : a zone
 \return hash value for zone
 */
inline std::size_t hash_value(tchecker::gtazg::gta_zone_t const & zone) { return zone.hash(); }

/*!
\brief Lexical comparison of gta zones
\param z1 : a gta_zone
\param z2 : a gta_zone
\return see tchecker::gtazg::gta_zone_t::lexical_cmp
 */
inline int lexical_cmp(tchecker::gtazg::gta_zone_t const & z1, tchecker::gtazg::gta_zone_t const & z2)
{
  return z1.lexical_cmp(z2);
}

/*!
\brief Mutual simulation check for zones
\param z1 : a gta_zone
\param z2 : a gta_zone
\param bounds : gsim clockbounds corresponding to the vloc of zone1 and zone2
\return true if z1 ~=_G z2 and false otherwise
 */
bool is_mutual_sim(tchecker::gtazg::gta_zone_t const & zone1, tchecker::gtazg::gta_zone_t const & zone2,
                   tchecker::clockbounds::gta::gsim_map_t & clockbounds);

} // end of namespace gtazg

/*!
 \class allocation_size_t
 \brief Specialization of class tchecker::allocation_size_t for type
 tchecker::gtazg::gta_zone_t
 */
template <> class allocation_size_t<tchecker::gtazg::gta_zone_t> {
public:
  /*!
   \brief Accessor
   \param dim : dimension
   \return Allocation size for objects of type tchecker::zg::zone_t
   with dimension dim
   */
  static constexpr std::size_t alloc_size(tchecker::clock_id_t dim)
  {
    return (sizeof(tchecker::gtazg::gta_zone_t) + dim * dim * sizeof(tchecker::dbm::extended::db_t));
  }

  /*!
   \brief Accessor
   \param dim : dimension
   \param args : arguments to a constructor of class tchecker::zg::zone_t
   \return Allocation size for objects of type tchecker::zg::zone_t
   with dimension dim
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(tchecker::clock_id_t dim, ARGS &&... args)
  {
    return allocation_size_t<tchecker::gtazg::gta_zone_t>::alloc_size(dim);
  }
};

// Allocation and deallocation

namespace gtazg {

/*!
 \brief Allocation and construction of DBM gta_zones
 \param dim : dimension
 \param args : arguments to a constructor of tchecker::gtazg::gta_zone_t
 \pre dim >= 1
 \return an instance of tchecker::gtazg::gta_zone_t of dimension dim and
 constructed with args
 \throw std::invalid_argument if dim < 1
 */
template <class... ARGS>
inline tchecker::gtazg::gta_zone_t * gta_zone_allocate_and_construct(tchecker::clock_id_t dim, ARGS &&... args)
{
  if (dim < 1)
    throw std::invalid_argument("dimension should be >= 1");

  void * ptr = new char[tchecker::allocation_size_t<tchecker::gtazg::gta_zone_t>::alloc_size(dim)];
  tchecker::gtazg::gta_zone_t::construct(ptr, args...);
  return reinterpret_cast<tchecker::gtazg::gta_zone_t *>(ptr);
}

/*!
 \brief Destruction and deallocation of DBM gta zones
 \param zone : a gta_zone
 \pre zone has been allocated by tchecker::gtazg::gta_zone_allocate_and_construct
 \post the destructor of zone has been called, and zone has been deleted
 */
void gta_zone_destruct_and_deallocate(tchecker::gtazg::gta_zone_t * zone);

} // end of namespace gtazg

/*!
 \brief Output a gta zone to a string
 \param zone : a gta zone
 \param index : a clock index
 \return a string corresponding to output of zone using index
 */
std::string to_string(tchecker::gtazg::gta_zone_t const & zone, tchecker::clock_index_t const & index);

} // end of namespace tchecker

#endif // TCHECKER_GTA_ZG_ZONE_HH
