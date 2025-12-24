/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTAZG_SEMANTICS_HH
#define TCHECKER_GTAZG_SEMANTICS_HH

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/gtadbm.hh"
#include "tchecker/gta_program/static_analysis.hh"

/*!
 \file semantics.hh
 \brief Operational semantics on GTA DBMs
 */

namespace tchecker {

namespace gtazg {

/*!
\class standard_semantics_t
\brief Standard semantics: each transition in the zone graph consists of a delay
(if allowed) followed by a transition from the generalized timed automaton
*/
class standard_semantics_t final {
public:
  /*!
  \brief Destructor
  */
  ~standard_semantics_t() = default;

  /*!
  \brief Compute initial zone
  \param dbm : a DBM over extended algebra
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in initial state
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param zero_history_clks_idx : Indices of history clks in dbm with initial value zero
 \param infinitiy_history_clks_idx : Indices of history clks in dbm with initial value infinity
  \post dbm is the intial gta zone with values of history clock set to their respective initial values, and prophecy clocks
 range from -inf to 0
 \return tchecker::STATE_OK
   */
  virtual tchecker::state_status_t initial(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                           tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                           tchecker::gtadbm::history_clks_idx_t const & zero_history_clks_idx,
                                           tchecker::gtadbm::history_clks_idx_t const & infinitiy_history_clks_idx);

  /*!
  \brief Pass time before edge execution according to the semantics
  \param dbm : a DBM over extended algebra
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay is allowed in source location
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
  */
  void delay_before_transition(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                               tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                               tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);

  /*!
  \brief Compute zone after execution of gta program
  \param dbm : a DBM over extended algebra
  \param dim : dimension of dbm
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
 \param gta_prog : Compiled gta program
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection
  with some guard of gta program result in an empty zone
  */
  tchecker::state_status_t execute_gta_program(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                               tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                               tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                                               tchecker::gta::compiled_gta_program_t const & gta_prog);

  /*!
  \brief Pass time after edge execution according to the semantics
  \param dbm : a DBM over extended algebra
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay is allowed in source location
 \param prophecy_clks_idx : Indices of prophecy clks in dbm
 \param history_clks_idx : Indices of history clks in dbm
  */
  void delay_after_transition(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, bool tgt_delay_allowed,
                              tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                              tchecker::gtadbm::history_clks_idx_t const & history_clks_idx);
};

tchecker::gtazg::standard_semantics_t * semantics_factory();

} // namespace gtazg

} // end of namespace tchecker

#endif // TCHECKER_GTAZG_SEMANTICS_HH
