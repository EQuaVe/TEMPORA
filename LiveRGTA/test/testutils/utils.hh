/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TESTUTILS_UTILS_HH
#define TCHECKER_TESTUTILS_UTILS_HH

#include <memory>
#include <string>

#include "tchecker/gta_program/gta_program.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/utils/log.hh"

/*!
 \file utils.hh
 \brief Utilities for unit tests
 */

namespace tchecker {

namespace test {

/*!
 \brief Parse a model
 \param model : tchecker model
 \return the system declaration corresponding to model if model is syntactically correct, nullptr otherwise or if
 a temporary file cannot be created
 \post errors and warnings in model have been reported to std::cerr
 */
std::shared_ptr<tchecker::parsing::system_declaration_t> parse(std::string const & model);

/*!
 \brief Parse a gta program
 \param gta_program : GTA program to parse
 \return void
 \post If unable to parse, error reported on cerr and exception thrown
 */
std::shared_ptr<tchecker::gta::gta_program_t> parse_gta(std::string const & gta_program);
} // end of namespace test

} // end of namespace tchecker

#endif // TCHECKER_TESTUTILS_UTILS_HH
