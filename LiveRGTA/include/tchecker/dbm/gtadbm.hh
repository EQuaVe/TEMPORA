/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_DBM_HH
#define TCHECKER_GTA_DBM_HH

#include <boost/container/flat_set.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/dbm.hh"

/*!
 * \file gtadbm.hh
 * \brief DBM for the zones of GTA
 *  \note These DBM use difference bounds over extended algebra. Along with
 *  the DBM (which is an array of tchecker::dbm::extended::db_t) and the dimension
 *  of the DBM, the user must also pass in the set of indices in DBM corresponding
 *  to prophecy and history clocks. The index of a clk with id clk_id is clk_id + 1
 */
namespace tchecker {

namespace gtadbm {

/*!
 * \brief Type to represent an index in dbm
 * \note We differentiate the two types to remind the user that the index of a clock
 * is 1 + clk_id */
using clk_idx_t = tchecker::clock_id_t;
/*!
 * \class prophecy_clks_idx_t
 * \brief Wrapper class around unordered_set to represent the set of indices with prophecy clocks
 */
class prophecy_clks_idx_t : public boost::container::flat_set<tchecker::clock_id_t> {
public:
  /*!
   * \brief Constructor
   * \param prophecy_clks : Bitset representing the ids of prophecy clks
   */
  prophecy_clks_idx_t(boost::dynamic_bitset<> const & prophecy_clks);
};

/*!
 * \class history_clks_idx_t
 * \brief Wrapper class around unordered_set to represent the set of indices with history clocks
 */
class history_clks_idx_t : public boost::container::flat_set<tchecker::clock_id_t> {
public:
  /*!
   * \brief Constructor
   * \param history_clks : Bitset representing the ids of history clks
   */
  history_clks_idx_t(boost::dynamic_bitset<> const & history_clks);
};

/*!
 \brief Copy a DBM into another DBM
 \param dbm1 : target dbm
 \param dbm2 : source dbm
 \param dim : dimension of dbm1 and dbm2
 \pre dbm1 is not nullptr (checked by assertion)
 dbm1 is a dim*dim array of difference bounds
 dbm2 is not nullptr (checked by assertion)
 dbm2 is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm1 is a copy of dbm2
*/
void copy(tchecker::dbm::extended::db_t * dbm1, tchecker::dbm::extended::db_t const * dbm2, tchecker::clock_id_t dim);

/*!
 \brief Equality predicate
 \param dbm1 : a first dbm
 \param dbm2 : a second dbm
 \param dim : dimension of dbm1 and dbm2
 \param prophecy_clks_idx : Indices of prophecy clks in dbm1 and dbm2
 \param history_clks_idx : Indices of history clks in dbm1 and dbm2
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are tight and in standard form (checked by assertion)
 dim >= 1 (checked by assertion).
 \return true if dbm1 and dbm2 are equal, false otherwise
 */
bool is_equal(tchecker::dbm::extended::db_t const * dbm1, tchecker::dbm::extended::db_t const * dbm2, tchecker::clock_id_t dim,
              tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
              tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Universal zone of a GTA
 \param dbm : a DBM
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm represents the universal dbm where all prophecy clks are non-positive
 and all history clocks are non-negative
 */
void gta_universal(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                   tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                   tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Empty gta zone
 \param dbm : a DBM
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm represents the empty zone (is_empty_0() returns true on dbm)
 dbm IS NOT TIGHT (empty DBMs cannot be tight)
 */
void empty(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
           tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
           tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Initialize to initial zone dbm
 \param dbm : a DBM over extended algebra
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param zero_history_clks_idx : Indices of history clks in dbm with initial value zero
 \param infinitiy_history_clks_idx : Indices of history clks in dbm with initial value infinity
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm represents the initial zone based on initial values for history clocks and is tight
 */
void init(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
          tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
          tchecker::gtadbm::history_clks_idx_t const & zero_history_clks_idx,
          tchecker::gtadbm::history_clks_idx_t const & infinitiy_history_clks_idx);

/*!
 \brief Fast emptiness predicate
 \param dbm : a gta DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm has been tightened
 dim >= 1 (checked by assertion).
 \return true if dbm has a negative difference bound on (0,0), false otherwise
 \note this function only checks the (0,0) entry of dbm. Hence, dbm may be empty but not be detected by this function. However,
 all other functions set (0,0) to a value less-than <=0 when they generate an empty zone. So this function can be used as a
 safe and efficient emptiness check if dbm has been generated by calls to functions in this file.
 */
bool is_empty_0(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Checks if the DBM is in standard form
 \param dbm : a DBM
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \return true if dbm is in standard form, false otherwise
 */
bool is_standard_form(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim,
                      tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                      tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Tightness predicate
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \return true if dbm is tight, false otherwise
 */
bool is_tight(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Tighten a DBM
 \param dbm : a DBM
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is in standard form (checked by assertion)
 \post dbm is not nullptr (checked by assertion)
 dbm is tight if it is not empty.
 if dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
 \return EMPTY if dbm is empty, NON_EMPTY otherwise
 \note Applies Floyd-Warshall algorithm on dbm seen as a weighted graph.
 */
enum tchecker::dbm::status_t tighten(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                     tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                     tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Tighten a DBM w.r.t. a constraint
 \param dbm : a DBM
 \param dim : dimension of dbm
 \param x : index of first clock
 \param y : index of second clock
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 0 <= x < dim (checked by assertion)
 0 <= y < dim (checked by assertion)
 x != y (checked by assertion)
 \post for all clocks u and v, the edge u->v in the graph is tight w.r.t. the
 edge y->x. That is, the length of the path u->v is the minimum between its
 length before the call and the length of the path u->y->x->v.
 if dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::gtadbm::is_empty_0() returns true)
 \return EMPTY if dbm is empty, MAY_BE_EMPTY otherwise
 \note if every edge in dbm is tight w.r.t. all other edges except i->j, then after the call, dbm is either empty, or it is
 not empty and tight.
 */
enum tchecker::dbm::status_t tighten(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                     tchecker::gtadbm::clk_idx_t x, tchecker::gtadbm::clk_idx_t y,
                                     tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                     tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Tighten the upper bounds of clocks in a DBM
 \param dbm : a DBM
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is in standard form (checked by assertion)
 \post dbm is not nullptr (checked by assertion)
 uppper bounds have been tightened
 if dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
 \note Performs Floyd-Warshall algorithm but only for upper bounds
 */
void tighten_upper_bounds(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                          tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                          tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Constrain a DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : first clock idx
 \param y : second clock idx
 \param cmp : constraint comparator
 \param value : constraint value
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 0 <= y < dim (checked by assertion)
 value can be represented in a tchecker::dbm::extende::db_t
 \post dbm has been intersected with constraint `x - y # value` where # is < if cmp is LT, and # is <= if cmp is LE
 dbm is tight if it is not empty.
 if dbm is empty, then its difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
 \return EMPTY if dbm is empty, NON_EMPTY otherwise
 \throw std::invalid_argument : if `cmp value` cannot be represented as a tchecker::dbm::extended::db_t
 */
enum tchecker::dbm::status_t constrain(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::gtadbm::clk_idx_t x, tchecker::gtadbm::clk_idx_t y, tchecker::ineq_cmp_t cmp,
                                       tchecker::integer_t value,
                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                       tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Constrain a DBM w.r.t. a clock constraint
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param cc : clock constraint
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 cc is expressed over system clocks (i.e. the reference clock is tchecker::REFCLOCK_ID)
 \post dbm has been intersected with cc
 if dbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::dbm::is_empty_0() returns true)
 the resulting DBM is tight and in standard if not empty
 \return EMPTY is the resulting DBM is empty, NON_EMPTY otherwise
*/
enum tchecker::dbm::status_t constrain(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::clock_constraint_t const & cc,
                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                       tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Constrain a DBM w.r.t. clock constraints container
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param constraints : clock constraints
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 all clock constraints in constraints are expressed over system clocks
 \post dbm has been intersected with constraints
 if dbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::dbm::is_empty_0() returns true)
 the resulting DBM is tight and in standard form if not empty
 \return EMPTY is the resulting DBM is empty, NON_EMPTY otherwise
*/
enum tchecker::dbm::status_t constrain(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::clock_constraint_container_t const & constraints,
                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                       tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Reset a history clock
 \param dbm : a dbm over extended algebra
 \param dim : dimension of dbm
 \param x : index of history clk to be reset
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 x is a history clock (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 \post dbm has been updated according to reset `x := 0`
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 */
void reset(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, tchecker::gtadbm::clk_idx_t const x,
           tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
           tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief release a prophecy clock
 \param dbm : a dbm over extended algebra
 \param dim : dimension of dbm
 \param y : index of prophecy clk to be reset
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 y is a prophecy clock (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= y < dim (checked by assertion)
 \post dbm has been updated according to release `-inf <= y <= 0`
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 */
void release(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, tchecker::gtadbm::clk_idx_t const y,
             tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
             tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Rename operation for clocks: x = y
 \param dbm : a dbm over extended algebra
 \param dim : dimension of dbm
 \param x1 : index of lhs clk in the rename
 \param x2 : index of rhs clk in the rename
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 x and y are of the same type
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x1 < dim (checked by assertion)
 0 <= x2 < dim (checked by assertion)
 \post clock x has all the constraints of clock y
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 */
void rename(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, tchecker::gtadbm::clk_idx_t const x1,
            tchecker::gtadbm::clk_idx_t const x2, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
            tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Open up (delay)
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \post constraints on (finite valued) history clocks are relaxed to < inf, and on (finite valued) prophecy clocks to <= 0
 dbm is tight
 */
void open_up(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
             tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
             tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Checks if prophecy clock x can take value -inf in the dbm
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x: Index of clock in dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 x is a prophecy clock (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \return true if x can take value -inf
 */
bool can_be_minus_infinity(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim,
                           tchecker::gtadbm::clk_idx_t const x, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                           tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);
/*!
 \brief Checks if all prophecy clocks can take value -inf in the dbm
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is in standard form (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \return true if all prophecy clocks in the zone can take value -inf
 */
bool all_prop_clks_minus_inf(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                             tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                             tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

/*!
 \brief Checks inclusion w.r.t. gsim clockbounds
 \param dbm1 : a first dbm
 \param dbm2 : a second dbm
 \param dim : dimension of dbm1 and dbm2
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \param clockbounds : Gsim clockbounds
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are in standard form (checked by assertion)
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion)

 \note This is the algorithm presented in Akshay, S., Gastin, P., Govind, R., Joshi, A.R., Srivathsan, B.:
 "A Unified Model for Real-Time Systems: Symbolic Techniques and Implementation". CAV 2023
 \return true if dbm1 <=G dbm2, false otherwise (see "A Unified Model for Real-Time Systems: Symbolic Techniques and
 Implementation" by Akshay, S., Gastin, P., Govind, R., Joshi, A.R., Srivathsan, B. CAV '23)
 */
bool is_gmap_le(tchecker::dbm::extended::db_t * dbm1, tchecker::dbm::extended::db_t * dbm2, tchecker::clock_id_t dim,
                tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                tchecker::clockbounds::gta::gsim_map_t & clockbounds);

/*!
 \brief Checks mutual simulation w.r.t. gsim clockbounds
 \param dbm1 : a first dbm
 \param dbm2 : a second dbm
 \param dim : dimension of dbm1 and dbm2
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \param clockbounds : Gsim clockbounds
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are in standard form (checked by assertion)
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion)

 \return true if dbm1 <=G dbm2 && dbm2 <=G dbm1, false otherwise
 */
bool is_gmap_mutual_sim(tchecker::dbm::extended::db_t const * dbm1, tchecker::dbm::extended::db_t const * dbm2,
                        tchecker::clock_id_t dim, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                        tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                        tchecker::clockbounds::gta::gsim_map_t & clockbounds);

/*!
 \brief Lexical ordering
 \param dbm1 : first dbm
 \param dim1 : dimension of dbm1
 \param dbm2 : second dbm
 \param dim2 : dimension of dbm2
 \pre dbm1 and dbm2 are ot nullptr (checked by assertion)
 dbm1 is a dim1*dim1 array of difference bounds
 dbm2 is a dim2*dim2 array of difference bounds
 dim1 >= 1 and dim2 >= 1 (checked by assertion)
 \return 0 if dbm1 and dbm2 are equal, a negative value if dbm1 is smaller than dbm2 w.r.t. lexical ordering, and a positive
 value otherwise
 */
int lexical_cmp(tchecker::dbm::extended::db_t const * dbm1, tchecker::clock_id_t dim1,
                tchecker::dbm::extended::db_t const * dbm2, tchecker::clock_id_t dim2);

/*!
 \brief Hash function
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \return hash code of dbm
 */
std::size_t hash(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Partial Hash function
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \return hash code of lower bounds for prophecy clocks
 */
std::size_t hash_prophecy_lower_bounds(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim,
                                       prophecy_clks_idx_t const & prophecy_clks_idx);

/*!
 \brief Output a DBM as a conjunction of constraints
 \param os : output stream
 \param dbm : a dbm over extended algebra
 \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \param clock_name : map from clock IDs to strings
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion).
 clock_name maps any clock ID in [0,dim) to a name
 \post the relevant constraints in dbm has been output to os. Relevant constraints are those that differ from the
 universal DBM.
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                      tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                      tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                      std::function<std::string(tchecker::clock_id_t)> clock_name);
} // end of namespace gtadbm

} // end of namespace tchecker

#endif // TCHECKER_GTA_DBM_HH
