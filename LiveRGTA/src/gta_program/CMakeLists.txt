# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(GTA_PROGRAM_SRC
${CMAKE_CURRENT_SOURCE_DIR}/guard.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/guard.hh
${CMAKE_CURRENT_SOURCE_DIR}/typed_guard.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/typed_guard.hh
${CMAKE_CURRENT_SOURCE_DIR}/action.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/action.hh
${CMAKE_CURRENT_SOURCE_DIR}/typed_action.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/typed_action.hh
${CMAKE_CURRENT_SOURCE_DIR}/gta_program.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/gta_program.hh
${CMAKE_CURRENT_SOURCE_DIR}/typed_gta_program.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/typed_gta_program.hh
${CMAKE_CURRENT_SOURCE_DIR}/typechecking.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/typechecking.hh
${CMAKE_CURRENT_SOURCE_DIR}/static_analysis.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta_program/static_analysis.hh
PARENT_SCOPE)
