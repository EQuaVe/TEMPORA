/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cstring>

#include "tchecker/dbm/gtadbm.hh"
#include "tchecker/gta_program/static_analysis.hh"
#include "tchecker/utils/ordering.hh"

namespace tchecker {

namespace gtadbm {

#define DBM(i, j)  dbm[(i) * dim + (j)]
#define DBM1(i, j) dbm1[(i) * dim + (j)]
#define DBM2(i, j) dbm2[(i) * dim + (j)]

// prophecy_clks_idx_t
prophecy_clks_idx_t::prophecy_clks_idx_t(boost::dynamic_bitset<> const & prophecy_clks)
{
  for (size_t p_clk_id = prophecy_clks.find_first(); p_clk_id != boost::dynamic_bitset<>::npos;
       p_clk_id = prophecy_clks.find_next(p_clk_id)) {
    // index of clock = id + 1
    this->insert(p_clk_id + 1);
  }
}

// history_clks_idx_t
history_clks_idx_t::history_clks_idx_t(boost::dynamic_bitset<> const & history_clks)
{
  for (size_t h_clk_idx = history_clks.find_first(); h_clk_idx != boost::dynamic_bitset<>::npos;
       h_clk_idx = history_clks.find_next(h_clk_idx)) {
    // index of clock = id + 1
    this->insert(h_clk_idx + 1);
  }
}

void copy(tchecker::dbm::extended::db_t * dbm1, tchecker::dbm::extended::db_t const * dbm2, tchecker::clock_id_t dim)
{
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(dim >= 1);

  std::memcpy(dbm1, dbm2, dim * dim * sizeof(*dbm2));
}

bool is_equal(tchecker::dbm::extended::db_t const * dbm1, tchecker::dbm::extended::db_t const * dbm2, tchecker::clock_id_t dim,
              tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
              tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm1, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm1, dim));

  assert(tchecker::gtadbm::is_standard_form(dbm2, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm2, dim));

  for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
    for (tchecker::gtadbm::clk_idx_t j = 0; j < dim; ++j) {
      if (DBM1(i, j) != DBM2(i, j)) {
        return false;
      }
    }
  }
  return true;
}
void gta_universal(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                   tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                   tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);

  DBM(0, 0) = tchecker::dbm::extended::LE_ZERO;

  for (tchecker::gtadbm::clk_idx_t const p1_idx : prophecy_clks_idx) {
    // -inf <= x <= 0
    DBM(p1_idx, 0) = tchecker::dbm::extended::LE_ZERO;
    DBM(0, p1_idx) = tchecker::dbm::extended::LE_INFINITY;

    for (tchecker::gtadbm::clk_idx_t const h2_idx : history_clks_idx) {
      // x1 - y2 <= 0
      DBM(p1_idx, h2_idx) = tchecker::dbm::extended::LE_ZERO;
    }

    for (tchecker::gtadbm::clk_idx_t const p2_idx : prophecy_clks_idx) {
      // x1 - x2 <= inf
      DBM(p1_idx, p2_idx) = tchecker::dbm::extended::LE_INFINITY;
    }

    // x - x <= inf as x can be equal to -inf
    DBM(p1_idx, p1_idx) = tchecker::dbm::extended::LE_INFINITY;
  }

  for (tchecker::gtadbm::clk_idx_t const h1_idx : history_clks_idx) {
    // 0 <= y <= inf
    DBM(h1_idx, 0) = tchecker::dbm::extended::LE_INFINITY;
    DBM(0, h1_idx) = tchecker::dbm::extended::LE_ZERO;

    for (tchecker::gtadbm::clk_idx_t const p2_idx : prophecy_clks_idx) {
      // y1 - x2 <= inf
      DBM(h1_idx, p2_idx) = tchecker::dbm::extended::LE_INFINITY;
    }

    for (tchecker::gtadbm::clk_idx_t const h2_idx : history_clks_idx) {
      // y1 - y2 <= inf
      DBM(h1_idx, h2_idx) = tchecker::dbm::extended::LE_INFINITY;
    }

    // y - y <= inf as y can be equal to inf
    DBM(h1_idx, h1_idx) = tchecker::dbm::extended::LE_INFINITY;
  }

  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
}

void empty(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
           tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
           tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  tchecker::gtadbm::gta_universal(dbm, dim, prophecy_clks_idx, history_clks_idx);
  DBM(0, 0) = tchecker::dbm::extended::LT_ZERO;
  assert(tchecker::gtadbm::is_empty_0(dbm, dim));
}

void init(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
          tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
          tchecker::gtadbm::history_clks_idx_t const & zero_history_clks_idx,
          tchecker::gtadbm::history_clks_idx_t const & infinitiy_history_clks_idx)
{

  assert(dbm != nullptr);
  assert(dim >= 1);

  DBM(0, 0) = tchecker::dbm::extended::LE_ZERO;

  for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
    // -inf <= x <= 0
    DBM(x, 0) = tchecker::dbm::extended::LE_ZERO;
    DBM(0, x) = tchecker::dbm::extended::LE_INFINITY;

    for (tchecker::gtadbm::clk_idx_t const x2 : prophecy_clks_idx) {
      // x1 - x2 <= inf
      DBM(x, x2) = tchecker::dbm::extended::LE_INFINITY;
    }

    for (tchecker::gtadbm::clk_idx_t const y0 : zero_history_clks_idx) {
      // x - 0 <= 0
      DBM(x, y0) = tchecker::dbm::extended::LE_ZERO;
    }

    for (tchecker::gtadbm::clk_idx_t const yinf : infinitiy_history_clks_idx) {
      // x - inf <= -inf
      DBM(x, yinf) = tchecker::dbm::extended::LE_MINUS_INFINITY;
    }
  }

  for (tchecker::gtadbm::clk_idx_t const y : zero_history_clks_idx) {
    // 0 <= y <= 0
    DBM(y, 0) = tchecker::dbm::extended::LE_ZERO;
    DBM(0, y) = tchecker::dbm::extended::LE_ZERO;

    for (tchecker::gtadbm::clk_idx_t const y2 : zero_history_clks_idx) {
      // 0 - 0 <= 0
      DBM(y, y2) = tchecker::dbm::extended::LE_ZERO;
    }

    for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
      // 0 - x <= inf
      DBM(y, x) = tchecker::dbm::extended::LE_INFINITY;
    }

    for (tchecker::gtadbm::clk_idx_t const yinf : infinitiy_history_clks_idx) {
      // 0 - inf <= -inf
      DBM(y, yinf) = tchecker::dbm::extended::LE_MINUS_INFINITY;
    }
  }

  for (tchecker::gtadbm::clk_idx_t const y : infinitiy_history_clks_idx) {
    // inf <= y <= inf
    DBM(y, 0) = tchecker::dbm::extended::LE_INFINITY;
    DBM(0, y) = tchecker::dbm::extended::LE_MINUS_INFINITY;

    for (tchecker::gtadbm::clk_idx_t const y2 : infinitiy_history_clks_idx) {
      // inf - inf <= inf
      DBM(y, y2) = tchecker::dbm::extended::LE_INFINITY;
    }

    for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
      // inf - x <= inf
      DBM(y, x) = tchecker::dbm::extended::LE_INFINITY;
    }

    for (tchecker::gtadbm::clk_idx_t const y0 : zero_history_clks_idx) {
      // inf - 0 <= inf
      DBM(y, y0) = tchecker::dbm::extended::LE_INFINITY;
    }
  }

  assert(tchecker::gtadbm::is_tight(dbm, dim));
}

bool is_empty_0(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  return (DBM(0, 0) < tchecker::dbm::extended::LE_ZERO);
}

bool is_standard_form(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim,
                      tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                      tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);

  // We maintain this invariant for non-empty zones
  if (DBM(0, 0) != tchecker::dbm::extended::LE_ZERO) {
    return false;
  }

  // prophecy clocks are non-positive
  for (tchecker::gtadbm::clk_idx_t const p_idx : prophecy_clks_idx) {
    if (DBM(p_idx, 0) > tchecker::dbm::extended::LE_ZERO || DBM(0, p_idx) < tchecker::dbm::extended::LE_ZERO) {
      return false;
    }

    bool const cannot_be_minus_infinity = (DBM(0, p_idx) < tchecker::dbm::extended::LE_INFINITY);
    bool const is_diagonal_le_zero = (DBM(p_idx, p_idx) == tchecker::dbm::extended::LE_ZERO);
    bool const is_diagonal_le_inf = (DBM(p_idx, p_idx) == tchecker::dbm::extended::LE_INFINITY);

    // diagonal is either <= 0 or <= inf
    if (!(is_diagonal_le_inf || is_diagonal_le_zero)) {
      return false;
    }

    // if x is not -inf, x-x <= 0
    if (cannot_be_minus_infinity && is_diagonal_le_inf) {
      return false;
    }
  }

  // history clocks are non-negative
  for (tchecker::gtadbm::clk_idx_t const h_idx : history_clks_idx) {
    if (DBM(h_idx, 0) < tchecker::dbm::extended::LE_ZERO || DBM(0, h_idx) > tchecker::dbm::extended::LE_ZERO) {
      return false;
    }

    bool const cannot_be_infinity = (DBM(h_idx, 0) < tchecker::dbm::extended::LE_INFINITY);
    bool const is_diagonal_le_zero = (DBM(h_idx, h_idx) == tchecker::dbm::extended::LE_ZERO);
    bool const is_diagonal_le_inf = (DBM(h_idx, h_idx) == tchecker::dbm::extended::LE_INFINITY);

    // diagonal is either <= 0 or <= inf
    if (!(is_diagonal_le_inf || is_diagonal_le_zero)) {
      return false;
    }

    // if y is not inf, y-y <= 0
    if (cannot_be_infinity && is_diagonal_le_inf) {
      return false;
    }
  }

  for (tchecker::gtadbm::clk_idx_t x_idx = 1; x_idx < dim; x_idx++) {
    // If x - y is at-most a finite value, x and -y cannot be inf
    for (tchecker::gtadbm::clk_idx_t y_idx = 1; y_idx < dim; y_idx++) {
      if (DBM(x_idx, y_idx) != tchecker::dbm::extended::LE_INFINITY) {
        if (DBM(x_idx, 0) == tchecker::dbm::extended::LE_INFINITY || DBM(0, y_idx) == tchecker::dbm::extended::LE_INFINITY) {
          return false;
        }
      }
    }
  }

  return true;
}

bool is_tight(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
    for (tchecker::gtadbm::clk_idx_t j = 0; j < dim; ++j) {
      for (tchecker::gtadbm::clk_idx_t k = 0; k < dim; ++k) {
        if (tchecker::dbm::extended::sum(DBM(i, k), DBM(k, j)) < DBM(i, j)) {
          return false;
        }
      }
    }
  }
  return true;
}

enum tchecker::dbm::status_t tighten(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                     tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                     tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));

  for (tchecker::gtadbm::clk_idx_t k = 0; k < dim; ++k) {
    for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
      if ((i == k) || (DBM(i, k) == tchecker::dbm::extended::LE_INFINITY)) { // optimization
        continue;
      }
      for (tchecker::gtadbm::clk_idx_t j = 0; j < dim; ++j) {
        DBM(i, j) = tchecker::dbm::extended::min(tchecker::dbm::extended::sum(DBM(i, k), DBM(k, j)), DBM(i, j));
      }
      if (DBM(i, i) < tchecker::dbm::extended::LE_ZERO) {
        DBM(0, 0) = tchecker::dbm::extended::LT_ZERO;
        return tchecker::dbm::EMPTY;
      }
    }
  }
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
  return tchecker::dbm::NON_EMPTY;
}

enum tchecker::dbm::status_t tighten(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                     tchecker::gtadbm::clk_idx_t x, tchecker::gtadbm::clk_idx_t y,
                                     tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                     tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(x < dim);
  assert(y < dim);

  if (DBM(x, y) == tchecker::dbm::extended::LE_INFINITY)
    return tchecker::dbm::MAY_BE_EMPTY;

  for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
    // tighten i->y w.r.t. i->x->y
    if (i != x) {
      tchecker::dbm::db_t db_ixy = tchecker::dbm::extended::sum(DBM(i, x), DBM(x, y));
      if (db_ixy < DBM(i, y)) {
        DBM(i, y) = db_ixy;
      }
    }

    // tighten i->j w.r.t. i->y->j
    for (tchecker::clock_id_t j = 0; j < dim; ++j) {
      DBM(i, j) = tchecker::dbm::extended::min(DBM(i, j), tchecker::dbm::extended::sum(DBM(i, y), DBM(y, j)));
    }

    if (DBM(i, i) < tchecker::dbm::extended::LE_ZERO) {
      DBM(0, 0) = tchecker::dbm::extended::LT_ZERO;
      return tchecker::dbm::EMPTY;
    }
  }

  return tchecker::dbm::MAY_BE_EMPTY;
}

void tighten_upper_bounds(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                          tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                          tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));

  for (tchecker::gtadbm::clk_idx_t k = 0; k < dim; ++k) {
    for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
      if ((i == k) || (DBM(i, k) == tchecker::dbm::extended::LE_INFINITY)) { // optimization
        continue;
      }
      DBM(i, 0) = tchecker::dbm::extended::min(tchecker::dbm::extended::sum(DBM(i, k), DBM(k, 0)), DBM(i, 0));
    }
  }
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::gtadbm::clk_idx_t x, tchecker::gtadbm::clk_idx_t y, tchecker::ineq_cmp_t cmp,
                                       tchecker::integer_t value,
                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                       tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
  assert(x < dim);
  assert(y < dim);

  tchecker::dbm::extended::db_t const db = tchecker::dbm::extended::db(cmp, value);
  tchecker::dbm::extended::db_t const old_db = DBM(x, y);
  if (db >= old_db)
    return tchecker::dbm::NON_EMPTY;

  // Since db < old_db, old_db = <=inf implies db < <=inf, and hence need to standarize
  bool const do_standarize = (old_db == tchecker::dbm::extended::LE_INFINITY);
  DBM(x, y) = db;

  if (do_standarize) {
    if (history_clks_idx.find(x) != history_clks_idx.end()) { // No need to update if x is a prophecy clock as x - 0 <=0
      // Update all the constraints x - i to be atmost <inf if i cannot be -inf and the constraint was was <= inf

      for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
        DBM(x, i) = tchecker::dbm::extended::min(tchecker::dbm::extended::sum(DBM(0, i), tchecker::dbm::extended::LT_INFINITY),
                                                 DBM(x, i));
      }
      DBM(x, x) = tchecker::dbm::extended::LE_ZERO;
    }

    if (prophecy_clks_idx.find(y) != prophecy_clks_idx.end()) { // No need to update if y is history clock as 0 - y <=0
      // Update all the constraints i - y to be atmost <inf if i cannot be inf and the constraint was was <= inf
      for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
        DBM(i, y) = tchecker::dbm::extended::min(tchecker::dbm::extended::sum(DBM(i, 0), tchecker::dbm::extended::LT_INFINITY),
                                                 DBM(i, y));
      }
      DBM(y, y) = tchecker::dbm::extended::LE_ZERO;
    }
  }

  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));

  tchecker::dbm::status_t res;

  res = tchecker::gtadbm::tighten(dbm, dim, x, y, prophecy_clks_idx, history_clks_idx);

  if (res == tchecker::dbm::MAY_BE_EMPTY)
    res = tchecker::dbm::NON_EMPTY; // since dbm was tight before

  assert((res == tchecker::dbm::EMPTY) || tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert((res == tchecker::dbm::EMPTY) || tchecker::gtadbm::is_tight(dbm, dim));

  return res;
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::clock_constraint_t const & cc,
                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                       tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));

  tchecker::gtadbm::clk_idx_t id1 = (cc.id1() == tchecker::REFCLOCK_ID ? 0 : cc.id1() + 1);
  tchecker::gtadbm::clk_idx_t id2 = (cc.id2() == tchecker::REFCLOCK_ID ? 0 : cc.id2() + 1);
  if (tchecker::gtadbm::constrain(dbm, dim, id1, id2, cc.comparator(), cc.value(), prophecy_clks_idx, history_clks_idx) ==
      tchecker::dbm::EMPTY)
    return tchecker::dbm::EMPTY;
  return tchecker::dbm::NON_EMPTY;
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::clock_constraint_container_t const & constraints,
                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                       tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{

  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));

  for (tchecker::clock_constraint_t const & cc : constraints) {
    if (tchecker::gtadbm::constrain(dbm, dim, cc, prophecy_clks_idx, history_clks_idx) == tchecker::dbm::EMPTY)
      return tchecker::dbm::EMPTY;
  }
  return tchecker::dbm::NON_EMPTY;
}

void reset(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, tchecker::gtadbm::clk_idx_t const x,
           tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
           tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(x < dim);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
  assert(history_clks_idx.find(x) != history_clks_idx.end());

  DBM(x, 0) = tchecker::dbm::extended::LE_ZERO;
  DBM(0, x) = tchecker::dbm::extended::LE_ZERO;
  for (tchecker::gtadbm::clk_idx_t i = 1; i < dim; ++i) {
    DBM(x, i) = DBM(0, i);
    DBM(i, x) = DBM(i, 0);
  }

  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
}

void release(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, tchecker::gtadbm::clk_idx_t const y,
             tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
             tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(y < dim);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
  assert(prophecy_clks_idx.find(y) != prophecy_clks_idx.end());

  DBM(y, 0) = tchecker::dbm::extended::LE_ZERO;
  DBM(0, y) = tchecker::dbm::extended::LE_INFINITY;

  for (tchecker::gtadbm::clk_idx_t i = 1; i < dim; ++i) {
    DBM(y, i) = DBM(0, i);
    DBM(i, y) = tchecker::dbm::extended::LE_INFINITY;
  }

  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
}

void rename(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, tchecker::gtadbm::clk_idx_t const x1,
            tchecker::gtadbm::clk_idx_t const x2, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
            tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(x1 < dim);
  assert(x2 < dim);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));

  for (tchecker::gtadbm::clk_idx_t i = 0; i < dim; ++i) {
    DBM(x1, i) = DBM(x2, i);
    DBM(i, x1) = DBM(i, x2);
  }
  DBM(x1, x1) = DBM(x2, x2);

  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
}

void open_up(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
             tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
             tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));

  for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
    if (DBM(x, 0) != tchecker::dbm::extended::LE_MINUS_INFINITY) {
      DBM(x, 0) = tchecker::dbm::extended::LE_ZERO;
    }
  }

  for (tchecker::gtadbm::clk_idx_t const y : history_clks_idx) {
    if (DBM(y, 0) < tchecker::dbm::extended::LT_INFINITY) {
      DBM(y, 0) = tchecker::dbm::extended::LT_INFINITY;
    }
  }

  // Unlike TA where we only have history clocks and so the delay can be arbitrary, in GTA we also have prophecy clocks which
  // have an upper bound of 0. This puts a constraint on the maximum possible delay. Hence we need to tighten the upper bounds
  tchecker::gtadbm::tighten_upper_bounds(dbm, dim, prophecy_clks_idx, history_clks_idx);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
}

bool can_be_minus_infinity(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim,
                           tchecker::gtadbm::clk_idx_t const x, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                           tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));
  assert(prophecy_clks_idx.find(x) != prophecy_clks_idx.end());

  return (DBM(0, x) == tchecker::dbm::extended::LE_INFINITY);
}

bool all_prop_clks_minus_inf(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                             tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                             tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::gtadbm::is_standard_form(dbm, dim, prophecy_clks_idx, history_clks_idx));
  assert(tchecker::gtadbm::is_tight(dbm, dim));

  for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
    if (DBM(0, x) < tchecker::dbm::extended::LE_INFINITY) {
      return false;
    }
  }

  return true;
}

bool is_diagonal_free_gmap_le(tchecker::dbm::extended::db_t const * dbm1, tchecker::dbm::extended::db_t const * dbm2,
                              tchecker::clock_id_t dim, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                              tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                              tchecker::clockbounds::gta::gsim_map_t const & clockbounds)
{
  for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
    if (DBM2(0, x) < DBM1(0, x)) {
      return false;
    }

    tchecker::clockbounds::gta::zero_minus_x_bound_t const & lower_bounds = clockbounds.zero_minus_x_bounds(x - 1);

    tchecker::dbm::extended::db_t const lower_bound = lower_bounds.greater_than_minus_inf_rvalue_bound();

    if (lower_bound == tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND) {
      continue;
    }

    if (DBM2(x, 0) < DBM1(x, 0)) {
      if (tchecker::dbm::extended::sum(DBM2(x, 0), lower_bound) < tchecker::dbm::extended::LE_ZERO) {
        return false;
      }
    }
  }

  for (tchecker::gtadbm::clk_idx_t const x : history_clks_idx) {
    tchecker::clockbounds::gta::x_minus_zero_bounds_t const & upper_bounds = clockbounds.x_minus_zero_bounds(x - 1);

    if (upper_bounds.contains_lt_inf_bound()) {
      if (DBM2(0, x) == tchecker::dbm::extended::LE_MINUS_INFINITY && DBM2(0, x) < DBM1(0, x)) {
        return false;
      }
    }

    tchecker::dbm::extended::db_t const upper_bound = upper_bounds.less_than_inf_rvalue_bound();

    if (upper_bound != tchecker::clockbounds::gta::X_MINUS_ZERO_NO_BOUND) {
      if (DBM2(0, x) < DBM1(0, x)) {
        if (tchecker::dbm::extended::sum(upper_bound, DBM1(0, x)) >= tchecker::dbm::extended::LE_ZERO) {
          return false;
        }
      }
    }

    tchecker::clockbounds::gta::zero_minus_x_bound_t const lower_bounds = clockbounds.zero_minus_x_bounds(x - 1);

    if (lower_bounds.contained_le_minus_inf_bound()) {
      if (DBM1(x, 0) == tchecker::dbm::extended::LE_INFINITY && DBM2(x, 0) < DBM1(x, 0)) {
        return false;
      }
    }

    tchecker::dbm::extended::db_t const lower_bound = lower_bounds.greater_than_minus_inf_rvalue_bound();

    if (lower_bound != tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND) {
      if (DBM2(x, 0) < DBM1(x, 0)) {
        if (tchecker::dbm::extended::sum(DBM2(x, 0), lower_bound) < tchecker::dbm::extended::LE_ZERO) {
          return false;
        }
      }
    }
  }

  std::size_t const num_clks = clockbounds.num_clks();

  for (tchecker::clock_id_t x = 0; x < num_clks; x++) {
    tchecker::gtadbm::clk_idx_t const x_idx = x + 1;

    tchecker::dbm::extended::db_t const upper_bound = clockbounds.x_minus_zero_bounds(x).less_than_inf_rvalue_bound();

    if (upper_bound == tchecker::clockbounds::gta::X_MINUS_ZERO_NO_BOUND) {
      continue;
    }

    tchecker::integer_t const upper_bound_val = upper_bound.value;
    if (upper_bound_val == tchecker::dbm::extended::MINUS_INF_VALUE) {
      continue;
    }

    for (tchecker::clock_id_t y = 0; y < num_clks; y++) {
      if (x == y) {
        continue;
      }

      tchecker::gtadbm::clk_idx_t const y_idx = y + 1;

      tchecker::dbm::extended::db_t const lower_bound =
          clockbounds.zero_minus_x_bounds(x).greater_than_minus_inf_rvalue_bound();

      if (lower_bound == tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND) {
        continue;
      }

      tchecker::integer_t const lower_bound_val = lower_bound.value;
      if (lower_bound_val == tchecker::dbm::extended::INF_VALUE) {
        continue;
      }

      if (DBM2(y_idx, x_idx) == tchecker::dbm::extended::LE_MINUS_INFINITY ||
          DBM2(y_idx, x_idx) == tchecker::dbm::extended::LE_INFINITY ||
          DBM2(y_idx, x_idx) == tchecker::dbm::extended::LT_INFINITY) {
        continue;
      }

      if (DBM2(y_idx, x_idx) < DBM1(y_idx, x_idx)) {
        if (tchecker::dbm::extended::sum(DBM1(0, x_idx), upper_bound) >= tchecker::dbm::extended::LE_ZERO &&
            tchecker::dbm::extended::sum(DBM2(y_idx, x_idx), lower_bound) < DBM1(0, x_idx)) {
          return false;
        }
      }
    }
  }

  return true;
}

bool is_gmap_le_aux(tchecker::dbm::extended::db_t const * dbm1, tchecker::dbm::extended::db_t const * dbm2,
                    tchecker::clock_id_t dim, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                    tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                    tchecker::clockbounds::gta::gsim_map_t & clockbounds)
{
  if (clockbounds.diagonal_bounds_empty()) {
    return true;
  }

  tchecker::clock_constraint_t const diag_bound = clockbounds.diagonal_pop();
  tchecker::clock_id_t x = diag_bound.id1();
  tchecker::clock_id_t y = diag_bound.id2();
  tchecker::dbm::extended::db_t bound = tchecker::dbm::extended::db(diag_bound.comparator(), diag_bound.value());

  // Check if dbm1 intersect phi <= dbm2 intersect phi
  tchecker::dbm::extended::db_t * dbm1_copy = new tchecker::dbm::extended::db_t[dim * dim];
  tchecker::gtadbm::copy(dbm1_copy, dbm1, dim);
  tchecker::gtadbm::constrain(dbm1_copy, dim, diag_bound, prophecy_clks_idx, history_clks_idx);

  tchecker::dbm::extended::db_t * dbm2_copy = new tchecker::dbm::extended::db_t[dim * dim];
  tchecker::gtadbm::copy(dbm2_copy, dbm2, dim);
  tchecker::gtadbm::constrain(dbm2_copy, dim, diag_bound, prophecy_clks_idx, history_clks_idx);

  if (not tchecker::gtadbm::is_gmap_le(dbm1_copy, dbm2_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
    // Cleanup
    clockbounds.add_diagonal_bound(x, y, bound);
    delete[] dbm1_copy;
    delete[] dbm2_copy;
    return false;
  }

  // Check if dbm1 intersect not phi <= dbm2
  if (bound == tchecker::dbm::extended::LE_INFINITY) {
    // Cleanup
    clockbounds.add_diagonal_bound(x, y, bound);
    delete[] dbm1_copy;
    delete[] dbm2_copy;

    return true;
  }

  tchecker::integer_t minus_c = tchecker::dbm::extended::negate_val(bound.value);
  tchecker::ineq_cmp_t negated_cmp = bound.cmp == tchecker::LE ? tchecker::LT : tchecker::LE;

  tchecker::gta::clk_type_t const x_type = (prophecy_clks_idx.find(x + 1) != prophecy_clks_idx.end())
                                               ? tchecker::gta::GTA_CLK_TYPE_PROPHECY
                                               : tchecker::gta::GTA_CLK_TYPE_HISTORY;
  tchecker::gta::clk_type_t const y_type = (prophecy_clks_idx.find(y + 1) != prophecy_clks_idx.end())
                                               ? tchecker::gta::GTA_CLK_TYPE_PROPHECY
                                               : tchecker::gta::GTA_CLK_TYPE_HISTORY;

  tchecker::clock_constraint_container_t negated_bound1 = tchecker::gta::diagonal_constraint_inf(x, x_type, y, y_type);

  // c <| x - y < inf
  tchecker::clock_constraint_container_t negated_bound2{
      tchecker::clock_constraint_t(x, y, tchecker::LT, tchecker::dbm::extended::INF_VALUE),
      tchecker::clock_constraint_t(y, x, negated_cmp, minus_c)};

  tchecker::gtadbm::copy(dbm1_copy, dbm1, dim);
  tchecker::gtadbm::copy(dbm2_copy, dbm2, dim);
  tchecker::gtadbm::constrain(dbm1_copy, dim, negated_bound1, prophecy_clks_idx, history_clks_idx);
  if (!tchecker::gtadbm::is_empty_0(dbm1_copy, dim)) {
    if (not tchecker::gtadbm::is_gmap_le_aux(dbm1_copy, dbm2_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
      // Cleanup
      clockbounds.add_diagonal_bound(x, y, bound);
      delete[] dbm1_copy;
      delete[] dbm2_copy;
      return false;
    }
  }

  tchecker::gtadbm::copy(dbm1_copy, dbm1, dim);
  tchecker::gtadbm::constrain(dbm1_copy, dim, negated_bound2, prophecy_clks_idx, history_clks_idx);
  if (!tchecker::gtadbm::is_empty_0(dbm1_copy, dim)) {
    bool const ret_val =
        tchecker::gtadbm::is_gmap_le_aux(dbm1_copy, dbm2_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds);
    // Cleanup
    clockbounds.add_diagonal_bound(x, y, bound);
    delete[] dbm1_copy;
    delete[] dbm2_copy;
    return ret_val;
  }
  // Cleanup
  clockbounds.add_diagonal_bound(x, y, bound);
  delete[] dbm1_copy;
  delete[] dbm2_copy;
  return true;
}

bool is_gmap_le(tchecker::dbm::extended::db_t * dbm1, tchecker::dbm::extended::db_t * dbm2, tchecker::clock_id_t dim,
                tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                tchecker::clockbounds::gta::gsim_map_t & clockbounds)
{
  if (tchecker::gtadbm::is_empty_0(dbm1, dim)) {
    return true;
  }

  if (tchecker::gtadbm::is_empty_0(dbm2, dim)) {
    return false;
  }

  if (not is_diagonal_free_gmap_le(dbm1, dbm2, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
    return false;
  }

  return is_gmap_le_aux(dbm1, dbm2, dim, prophecy_clks_idx, history_clks_idx, clockbounds);
}

bool is_diagonal_free_gmap_mutual_sim(tchecker::dbm::extended::db_t const * dbm1, tchecker::dbm::extended::db_t const * dbm2,
                                      tchecker::clock_id_t dim, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                      tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                                      tchecker::clockbounds::gta::gsim_map_t const & clockbounds)
{
  for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
    if (DBM2(0, x) != DBM1(0, x)) {
      return false;
    }

    tchecker::clockbounds::gta::zero_minus_x_bound_t const & lower_bounds = clockbounds.zero_minus_x_bounds(x - 1);

    tchecker::dbm::extended::db_t const lower_bound = lower_bounds.greater_than_minus_inf_rvalue_bound();

    if (lower_bound == tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND) {
      continue;
    }

    if (DBM2(x, 0) != DBM1(x, 0)) {
      if (tchecker::dbm::extended::sum(tchecker::dbm::extended::min(DBM1(x, 0), DBM2(x, 0)), lower_bound) <
          tchecker::dbm::extended::LE_ZERO) {
        return false;
      }
    }
  }

  for (tchecker::gtadbm::clk_idx_t const x : history_clks_idx) {
    tchecker::clockbounds::gta::x_minus_zero_bounds_t const & upper_bounds = clockbounds.x_minus_zero_bounds(x - 1);

    if (upper_bounds.contains_lt_inf_bound()) {
      if (DBM2(0, x) != DBM1(0, x)) {
        if (DBM2(0, x) == tchecker::dbm::extended::LE_MINUS_INFINITY ||
            DBM1(0, x) == tchecker::dbm::extended::LE_MINUS_INFINITY) {
          return false;
        }
      }
    }

    tchecker::dbm::extended::db_t const upper_bound = upper_bounds.less_than_inf_rvalue_bound();

    if (upper_bound != tchecker::clockbounds::gta::X_MINUS_ZERO_NO_BOUND) {
      if (DBM2(0, x) != DBM1(0, x)) {
        if (tchecker::dbm::extended::sum(upper_bound, tchecker::dbm::extended::max(DBM1(0, x), DBM2(0, x))) >=
            tchecker::dbm::extended::LE_ZERO) {
          return false;
        }
      }
    }

    tchecker::clockbounds::gta::zero_minus_x_bound_t const lower_bounds = clockbounds.zero_minus_x_bounds(x - 1);

    if (lower_bounds.contained_le_minus_inf_bound()) {
      if (DBM1(x, 0) != DBM2(x, 0)) {
        if (DBM1(x, 0) == tchecker::dbm::extended::LE_INFINITY || DBM2(x, 0) == tchecker::dbm::extended::LE_INFINITY) {
          return false;
        }
      }
    }

    tchecker::dbm::extended::db_t const lower_bound = lower_bounds.greater_than_minus_inf_rvalue_bound();

    if (lower_bound != tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND) {
      if (DBM2(x, 0) != DBM1(x, 0)) {
        if (tchecker::dbm::extended::sum(tchecker::dbm::extended::min(DBM1(x, 0), DBM2(x, 0)), lower_bound) <
            tchecker::dbm::extended::LE_ZERO) {
          return false;
        }
      }
    }
  }

  std::size_t const num_clks = clockbounds.num_clks();

  for (tchecker::clock_id_t x = 0; x < num_clks; x++) {
    tchecker::gtadbm::clk_idx_t const x_idx = x + 1;

    tchecker::dbm::extended::db_t const upper_bound = clockbounds.x_minus_zero_bounds(x).less_than_inf_rvalue_bound();

    if (upper_bound == tchecker::clockbounds::gta::X_MINUS_ZERO_NO_BOUND) {
      continue;
    }

    tchecker::integer_t const upper_bound_val = upper_bound.value;
    if (upper_bound_val == tchecker::dbm::extended::MINUS_INF_VALUE) {
      continue;
    }

    for (tchecker::clock_id_t y = 0; y < num_clks; y++) {
      if (x == y) {
        continue;
      }

      tchecker::gtadbm::clk_idx_t const y_idx = y + 1;

      tchecker::dbm::extended::db_t const lower_bound =
          clockbounds.zero_minus_x_bounds(x).greater_than_minus_inf_rvalue_bound();

      if (lower_bound == tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND) {
        continue;
      }

      tchecker::integer_t const lower_bound_val = lower_bound.value;
      if (lower_bound_val == tchecker::dbm::extended::INF_VALUE) {
        continue;
      }

      if (DBM2(y_idx, x_idx) == tchecker::dbm::extended::LE_MINUS_INFINITY ||
          DBM2(y_idx, x_idx) == tchecker::dbm::extended::LE_INFINITY ||
          DBM2(y_idx, x_idx) == tchecker::dbm::extended::LT_INFINITY) {
        continue;
      }

      if (DBM1(y_idx, x_idx) == tchecker::dbm::extended::LE_MINUS_INFINITY ||
          DBM1(y_idx, x_idx) == tchecker::dbm::extended::LE_INFINITY ||
          DBM1(y_idx, x_idx) == tchecker::dbm::extended::LT_INFINITY) {
        continue;
      }

      if (DBM1(0, x_idx) != DBM2(0, x_idx)) {
        continue;
      }

      if (DBM1(y_idx, x_idx) != DBM2(y_idx, x_idx)) {
        if (tchecker::dbm::extended::sum(DBM1(0, x_idx), upper_bound) >= tchecker::dbm::extended::LE_ZERO &&
            tchecker::dbm::extended::sum(tchecker::dbm::extended::min(DBM1(y_idx, x_idx), DBM2(y_idx, x_idx)), lower_bound) <
                DBM1(0, x_idx)) {
          return false;
        }
      }
    }
  }

  return true;
}

bool is_gmap_mutual_sim(tchecker::dbm::extended::db_t const * dbm1, tchecker::dbm::extended::db_t const * dbm2,
                        tchecker::clock_id_t dim, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                        tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                        tchecker::clockbounds::gta::gsim_map_t & clockbounds)
{
  if (tchecker::gtadbm::is_empty_0(dbm1, dim) && tchecker::gtadbm::is_empty_0(dbm2, dim)) {
    return true;
  }

  if (tchecker::gtadbm::is_empty_0(dbm1, dim) || tchecker::gtadbm::is_empty_0(dbm2, dim)) {
    return false;
  }

  if (not is_diagonal_free_gmap_mutual_sim(dbm1, dbm2, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
    return false;
  }

  if (clockbounds.diagonal_bounds_empty()) {
    return true;
  }

  tchecker::clock_constraint_t const diag_bound = clockbounds.diagonal_pop();
  tchecker::clock_id_t x = diag_bound.id1();
  tchecker::clock_id_t y = diag_bound.id2();
  tchecker::dbm::extended::db_t bound = tchecker::dbm::extended::db(diag_bound.comparator(), diag_bound.value());

  // Check if dbm1 intersect phi ~= dbm2 intersect phi
  tchecker::dbm::extended::db_t * dbm1_copy = new tchecker::dbm::extended::db_t[dim * dim];
  tchecker::gtadbm::copy(dbm1_copy, dbm1, dim);
  tchecker::gtadbm::constrain(dbm1_copy, dim, diag_bound, prophecy_clks_idx, history_clks_idx);

  tchecker::dbm::extended::db_t * dbm2_copy = new tchecker::dbm::extended::db_t[dim * dim];
  tchecker::gtadbm::copy(dbm2_copy, dbm2, dim);
  tchecker::gtadbm::constrain(dbm2_copy, dim, diag_bound, prophecy_clks_idx, history_clks_idx);

  if (not tchecker::gtadbm::is_gmap_mutual_sim(dbm1_copy, dbm2_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
    // Cleanup
    clockbounds.add_diagonal_bound(x, y, bound);
    delete[] dbm1_copy;
    delete[] dbm2_copy;
    return false;
  }

  if (bound == tchecker::dbm::extended::LE_INFINITY) {
    // Cleanup
    clockbounds.add_diagonal_bound(x, y, bound);
    delete[] dbm1_copy;
    delete[] dbm2_copy;
    return true;
  }

  tchecker::integer_t minus_c = tchecker::dbm::extended::negate_val(bound.value);
  tchecker::ineq_cmp_t negated_cmp = bound.cmp == tchecker::LE ? tchecker::LT : tchecker::LE;

  tchecker::gta::clk_type_t const x_type = (prophecy_clks_idx.find(x + 1) != prophecy_clks_idx.end())
                                               ? tchecker::gta::GTA_CLK_TYPE_PROPHECY
                                               : tchecker::gta::GTA_CLK_TYPE_HISTORY;
  tchecker::gta::clk_type_t const y_type = (prophecy_clks_idx.find(y + 1) != prophecy_clks_idx.end())
                                               ? tchecker::gta::GTA_CLK_TYPE_PROPHECY
                                               : tchecker::gta::GTA_CLK_TYPE_HISTORY;

  tchecker::clock_constraint_container_t negated_bound1 = tchecker::gta::diagonal_constraint_inf(x, x_type, y, y_type);

  // c <| x - y < inf
  tchecker::clock_constraint_container_t negated_bound2{
      tchecker::clock_constraint_t(x, y, tchecker::LT, tchecker::dbm::extended::INF_VALUE),
      tchecker::clock_constraint_t(y, x, negated_cmp, minus_c)};

  // Check if dbm1 intersect not phi <= dbm2
  tchecker::gtadbm::copy(dbm1_copy, dbm1, dim);
  tchecker::gtadbm::copy(dbm2_copy, dbm2, dim);
  tchecker::gtadbm::constrain(dbm1_copy, dim, negated_bound1, prophecy_clks_idx, history_clks_idx);
  if (!tchecker::gtadbm::is_empty_0(dbm1_copy, dim)) {
    if (!tchecker::gtadbm::is_gmap_le_aux(dbm1_copy, dbm2_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
      // Cleanup
      clockbounds.add_diagonal_bound(x, y, bound);
      delete[] dbm1_copy;
      delete[] dbm2_copy;
      return false;
    }
  }

  tchecker::gtadbm::copy(dbm1_copy, dbm1, dim);
  tchecker::gtadbm::constrain(dbm1_copy, dim, negated_bound2, prophecy_clks_idx, history_clks_idx);
  if (!tchecker::gtadbm::is_empty_0(dbm1_copy, dim)) {
    if (!tchecker::gtadbm::is_gmap_le_aux(dbm1_copy, dbm2_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
      // Cleanup
      clockbounds.add_diagonal_bound(x, y, bound);
      delete[] dbm1_copy;
      delete[] dbm2_copy;
      return false;
    }
  }

  // Check if dbm2 intersect not phi <= dbm1
  tchecker::gtadbm::copy(dbm1_copy, dbm1, dim);
  tchecker::gtadbm::copy(dbm2_copy, dbm2, dim);
  tchecker::gtadbm::constrain(dbm2_copy, dim, negated_bound1, prophecy_clks_idx, history_clks_idx);
  if (!tchecker::gtadbm::is_empty_0(dbm2_copy, dim)) {
    if (!tchecker::gtadbm::is_gmap_le_aux(dbm2_copy, dbm1_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
      // Cleanup
      clockbounds.add_diagonal_bound(x, y, bound);
      delete[] dbm1_copy;
      delete[] dbm2_copy;
      return false;
    }
  }

  tchecker::gtadbm::copy(dbm2_copy, dbm2, dim);
  tchecker::gtadbm::constrain(dbm2_copy, dim, negated_bound2, prophecy_clks_idx, history_clks_idx);
  if (!tchecker::gtadbm::is_empty_0(dbm2_copy, dim)) {
    if (!tchecker::gtadbm::is_gmap_le_aux(dbm2_copy, dbm1_copy, dim, prophecy_clks_idx, history_clks_idx, clockbounds)) {
      // Cleanup
      clockbounds.add_diagonal_bound(x, y, bound);
      delete[] dbm1_copy;
      delete[] dbm2_copy;
      return false;
    }
  }
  // Cleanup
  clockbounds.add_diagonal_bound(x, y, bound);
  delete[] dbm1_copy;
  delete[] dbm2_copy;
  return true;
}

int lexical_cmp(tchecker::dbm::extended::db_t const * dbm1, tchecker::clock_id_t dim1,
                tchecker::dbm::extended::db_t const * dbm2, tchecker::clock_id_t dim2)
{
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(dim1 >= 1);
  assert(dim2 >= 1);
  return tchecker::lexical_cmp(dbm1, dbm1 + dim1 * dim1, dbm2, dbm2 + dim2 * dim2, tchecker::dbm::extended::db_cmp);
}

std::size_t hash(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim)
{
  std::size_t seed = 0;
  for (tchecker::clock_id_t k = 0; k < dim * dim; ++k) {
    boost::hash_combine(seed, tchecker::dbm::extended::hash(dbm[k]));
  }
  return seed;
}

std::size_t hash_prophecy_lower_bounds(tchecker::dbm::extended::db_t const * dbm, tchecker::clock_id_t dim,
                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  std::size_t seed = 0;
  for (tchecker::gtadbm::clk_idx_t const x : prophecy_clks_idx) {
    boost::hash_combine(seed, tchecker::dbm::extended::hash(DBM(0, x)));
  }
  return seed;
}

std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                      tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                      tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                      std::function<std::string(tchecker::clock_id_t)> clock_name)
{
  assert(dbm != nullptr);
  assert(dim >= 1);

  os << "(";

  // output: x # c (first row/column)
  for (tchecker::gtadbm::clk_idx_t j = 1; j < dim; ++j) {
    tchecker::dbm::db_t c0j = DBM(0, j), cj0 = DBM(j, 0);
    tchecker::integer_t const c0j_val = tchecker::dbm::extended::value(c0j);
    tchecker::integer_t const cj0_val = tchecker::dbm::extended::value(cj0);
    if (j > 1) {
      os << " && ";
    }
    // xj = k
    if (cj0_val == tchecker::dbm::extended::negate_val(c0j_val)) {
      os << clock_name(j) << "==" << tchecker::dbm::extended::to_string(cj0_val);
    }
    // k1 <= xj <= k2
    else {
      if (c0j != tchecker::dbm::extended::LE_INFINITY) {
        os << tchecker::dbm::extended::to_string(tchecker::dbm::extended::negate_val(c0j_val));
        os << tchecker::dbm::extended::comparator_str(c0j);
      }
      os << clock_name(j);
      if (cj0 != tchecker::dbm::extended::LE_INFINITY) {
        tchecker::dbm::extended::output(os, cj0);
      }
    }
  }

  // output: x-y # c (other rows/columns)
  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    for (tchecker::clock_id_t j = i + 1; j < dim; ++j) {
      tchecker::dbm::db_t cij = DBM(i, j), cji = DBM(j, i);
      tchecker::integer_t const cij_val = tchecker::dbm::extended::value(cij);
      tchecker::integer_t const cji_val = tchecker::dbm::extended::value(cji);
      // xi == xj + k
      if (cij_val == tchecker::dbm::extended::negate_val(cji_val)) {
        os << " && ";
        os << clock_name(i) << "==" << clock_name(j);
        if (cij_val > 0) {
          os << "+" << tchecker::dbm::extended::to_string(cij_val);
        }
        else if (cij_val < 0) {
          os << tchecker::dbm::extended::to_string(cij_val);
        }
      }
      // k1 <= xi - xj <= k2
      else if ((cij != tchecker::dbm::extended::LE_INFINITY) || (cji != tchecker::dbm::extended::LE_INFINITY)) {
        os << " && ";

        if (cji != tchecker::dbm::extended::LE_INFINITY) {
          os << tchecker::dbm::extended::to_string(tchecker::dbm::extended::negate_val(cji_val))
             << tchecker::dbm::extended::comparator_str(cji);
        }

        os << clock_name(i) << "-" << clock_name(j);

        if (cij != tchecker::dbm::extended::LE_INFINITY) {
          os << tchecker::dbm::comparator_str(cij) << tchecker::dbm::extended::to_string(cij_val);
        }
      }
    }
  }

  os << ")";

  return os;
}
} // end of namespace gtadbm

} // end of namespace tchecker
