/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */
#include <boost/dynamic_bitset.hpp>

#include "tchecker/dbm/gtadbm.hh"

#define GDBM(i, j) gdbm[(i) * gdim + (j)]

TEST_CASE("universal", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 4;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm, gdim, prophecy_clks_idx, history_clks_idx);

  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));
}

TEST_CASE("empty", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 4;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::empty(gdbm, gdim, prophecy_clks_idx, history_clks_idx);

  REQUIRE(tchecker::gtadbm::is_empty_0(gdbm, gdim));
}

TEST_CASE("init", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 8;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  boost::dynamic_bitset<> zero_history_clks(num_clks);
  boost::dynamic_bitset<> inf_history_clks(num_clks);

  for (size_t i = 0; i < num_clks; i++) {
    if (i < num_clks / 2)
      prophecy_clks[i] = 1;
    else if (i < 3 * num_clks / 4)
      zero_history_clks[i] = 1;
    else
      inf_history_clks[i] = 1;
  }

  boost::dynamic_bitset<> history_clks = zero_history_clks | inf_history_clks;

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t zero_history_clks_idx(zero_history_clks);
  tchecker::gtadbm::history_clks_idx_t inf_history_clks_idx(inf_history_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(history_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::init(gdbm, gdim, prophecy_clks_idx, zero_history_clks_idx, inf_history_clks_idx);

  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));
}

TEST_CASE("tighten", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 6;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm, gdim, prophecy_clks_idx, history_clks_idx);

  GDBM(1, 2) = tchecker::dbm::extended::db(tchecker::LE, 2);
  GDBM(0, 2) = tchecker::dbm::extended::db(tchecker::LE, 5);
  GDBM(2, 2) = tchecker::dbm::extended::LE_ZERO;

  enum tchecker::dbm::status_t const res = tchecker::gtadbm::tighten(gdbm, gdim, prophecy_clks_idx, history_clks_idx);

  REQUIRE(res != tchecker::dbm::EMPTY);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));
}

TEST_CASE("constrain", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 6;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm, gdim, prophecy_clks_idx, history_clks_idx);
  enum tchecker::dbm::status_t res =
      tchecker::gtadbm::constrain(gdbm, gdim, 1, 2, tchecker::LE, 5, prophecy_clks_idx, history_clks_idx);

  REQUIRE(res != tchecker::dbm::EMPTY);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));

  res = tchecker::gtadbm::constrain(gdbm, gdim, 4, 0, tchecker::LT, 3, prophecy_clks_idx, history_clks_idx);

  REQUIRE(res != tchecker::dbm::EMPTY);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));

  res = tchecker::gtadbm::constrain(gdbm, gdim, 5, 3, tchecker::LT, 2, prophecy_clks_idx, history_clks_idx);

  REQUIRE(res != tchecker::dbm::EMPTY);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));

  res = tchecker::gtadbm::constrain(gdbm, gdim, 0, 6, tchecker::LT, 7, prophecy_clks_idx, history_clks_idx);

  REQUIRE(res != tchecker::dbm::EMPTY);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));

  res = tchecker::gtadbm::constrain(gdbm, gdim, 6, 4, tchecker::LE, -10, prophecy_clks_idx, history_clks_idx);

  REQUIRE(res == tchecker::dbm::EMPTY);
}

TEST_CASE("reset", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 2;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm, gdim, prophecy_clks_idx, history_clks_idx);
  tchecker::gtadbm::constrain(gdbm, gdim, 0, 2, tchecker::LT, -5, prophecy_clks_idx, history_clks_idx);

  tchecker::gtadbm::reset(gdbm, gdim, 2, prophecy_clks_idx, history_clks_idx);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));
}

TEST_CASE("release", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 2;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm, gdim, prophecy_clks_idx, history_clks_idx);
  tchecker::gtadbm::constrain(gdbm, gdim, 1, 0, tchecker::LT, -5, prophecy_clks_idx, history_clks_idx);

  tchecker::gtadbm::release(gdbm, gdim, 1, prophecy_clks_idx, history_clks_idx);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));
}

TEST_CASE("rename", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 6;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm, gdim, prophecy_clks_idx, history_clks_idx);
  tchecker::gtadbm::constrain(gdbm, gdim, 1, 0, tchecker::LT, -5, prophecy_clks_idx, history_clks_idx);
  tchecker::gtadbm::constrain(gdbm, gdim, 2, 1, tchecker::LT, -5, prophecy_clks_idx, history_clks_idx);

  tchecker::gtadbm::rename(gdbm, gdim, 3, 2, prophecy_clks_idx, history_clks_idx);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));
}

TEST_CASE("open-up", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 8;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  boost::dynamic_bitset<> zero_history_clks(num_clks);
  boost::dynamic_bitset<> inf_history_clks(num_clks);

  for (size_t i = 0; i < num_clks; i++) {
    if (i < num_clks / 2)
      prophecy_clks[i] = 1;
    else if (i < 3 * num_clks / 4)
      zero_history_clks[i] = 1;
    else
      inf_history_clks[i] = 1;
  }

  boost::dynamic_bitset<> history_clks = zero_history_clks | inf_history_clks;

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t zero_history_clks_idx(zero_history_clks);
  tchecker::gtadbm::history_clks_idx_t inf_history_clks_idx(inf_history_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(history_clks);

  tchecker::dbm::extended::db_t gdbm[gdim * gdim];
  tchecker::gtadbm::init(gdbm, gdim, prophecy_clks_idx, zero_history_clks_idx, inf_history_clks_idx);

  tchecker::gtadbm::open_up(gdbm, gdim, prophecy_clks, history_clks);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));

  tchecker::gtadbm::constrain(gdbm, gdim, 2, 0, tchecker::LE, -2, prophecy_clks, history_clks);
  tchecker::gtadbm::reset(gdbm, gdim, 7, prophecy_clks, history_clks);
  tchecker::gtadbm::constrain(gdbm, gdim, 6, 1, tchecker::LE, 3, prophecy_clks, history_clks);
  tchecker::gtadbm::constrain(gdbm, gdim, 4, 0, tchecker::LE, 2, prophecy_clks, history_clks);
  tchecker::gtadbm::constrain(gdbm, gdim, 3, 2, tchecker::LE, 4, prophecy_clks, history_clks);

  tchecker::gtadbm::open_up(gdbm, gdim, prophecy_clks, history_clks);
  REQUIRE(tchecker::gtadbm::is_standard_form(gdbm, gdim, prophecy_clks_idx, history_clks_idx));
  REQUIRE(tchecker::gtadbm::is_tight(gdbm, gdim));
}

TEST_CASE("equal", "[gtadbm]")
{

  tchecker::clock_id_t const num_clks = 4;
  tchecker::clock_id_t const gdim = num_clks + 1;

  boost::dynamic_bitset<> prophecy_clks(num_clks);
  for (size_t i = 0; i < num_clks / 2; i++) {
    prophecy_clks[i] = 1;
  }

  tchecker::gtadbm::prophecy_clks_idx_t prophecy_clks_idx(prophecy_clks);
  tchecker::gtadbm::history_clks_idx_t history_clks_idx(~prophecy_clks);

  tchecker::dbm::extended::db_t gdbm1[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm1, gdim, prophecy_clks_idx, history_clks_idx);
  tchecker::gtadbm::constrain(gdbm1, gdim, 1, 2, tchecker::LE, 5, prophecy_clks_idx, history_clks_idx);
  tchecker::gtadbm::constrain(gdbm1, gdim, 0, 1, tchecker::LE, 5, prophecy_clks_idx, history_clks_idx);

  tchecker::dbm::extended::db_t gdbm2[gdim * gdim];
  tchecker::gtadbm::gta_universal(gdbm2, gdim, prophecy_clks_idx, history_clks_idx);

  tchecker::gtadbm::constrain(gdbm2, gdim, 0, 1, tchecker::LE, 5, prophecy_clks_idx, history_clks_idx);
  REQUIRE(not tchecker::gtadbm::is_equal(gdbm1, gdbm2, gdim, prophecy_clks_idx, history_clks_idx));

  tchecker::gtadbm::constrain(gdbm2, gdim, 1, 2, tchecker::LE, 5, prophecy_clks_idx, history_clks_idx);
  REQUIRE(tchecker::gtadbm::is_equal(gdbm1, gdbm2, gdim, prophecy_clks_idx, history_clks_idx));
}
