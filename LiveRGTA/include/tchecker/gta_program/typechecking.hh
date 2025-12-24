/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef GTA_PROGRAM_TYPECHECKING_HH
#define GTA_PROGRAM_TYPECHECKING_HH

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/gta_program/gta_program.hh"
#include "tchecker/gta_program/typed_gta_program.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file typechecking.hh
 \brief Type checking for GTA programs
 */

namespace tchecker {

namespace gta {

/*!
 \brief Type checking for gta programs
 \param gta_prog : parsed gta program
 \param clocks : clock variables
 \param prophecy_clocks : The subset of clocks which are declared prophecy
 \return typed gta program, non-null if gta_prog can be assiged a valid type and null otherwise
 \post all errors have been reported to std::cerr
 */
std::shared_ptr<tchecker::gta::typed_gta_program_t> typechecking(std::shared_ptr<tchecker::gta::gta_program_t> const & gta_prog,
                                                                 tchecker::clock_variables_t const & clocks,
                                                                 boost::dynamic_bitset<> const & prophecy_clocks);

} // end of namespace gta

} // end of namespace tchecker

#endif // GTA_PROGRAM_TYPECHECKING_HH
