/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_GTA_SCC_STATS_HH
#define TCHECKER_ALGORITHMS_GTA_SCC_STATS_HH

#include "tchecker/algorithms/couvreur_scc/stats.hh"

/*!
 \file stats.hh
 \brief Statistics for Couvreur's SCC algorithm
 */

namespace tchecker {

namespace algorithms {

namespace gtascc {

/*!
 * \brief alias for stats in gta scc algorithm
 */
using stats_t = tchecker::algorithms::couvscc::stats_t;

} // end of namespace gtascc

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_GTA_SCC_STATS_HH
