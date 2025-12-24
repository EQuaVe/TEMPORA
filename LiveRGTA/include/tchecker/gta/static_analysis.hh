/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_STATIC_ANALYSIS_HH
#define TCHECKER_GTA_STATIC_ANALYSIS_HH

#include <boost/dynamic_bitset.hpp>

#include "tchecker/gta/system.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis of generalized timed automaton
 */

namespace tchecker {

namespace gta {

/*!
 * \brief computes if gta system is x_d safe
 * \param gta_system : GTA system
 * \returns the boolean representing if the system is x_d safe
 */
bool is_xd_safe(tchecker::gta::system_t const & gta_system);

} // end of namespace gta

} // end of namespace tchecker

#endif // TCHECKER_GTA_STATIC_ANALYSIS_HH
