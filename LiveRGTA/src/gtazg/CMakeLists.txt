
# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(GTAZG_SRC
${CMAKE_CURRENT_SOURCE_DIR}/gta_zone.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gtazg/gta_zone.hh
${CMAKE_CURRENT_SOURCE_DIR}/gtazg.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gtazg/gtazg.hh
${CMAKE_CURRENT_SOURCE_DIR}/semantics.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gtazg/semantics.hh
${CMAKE_CURRENT_SOURCE_DIR}/state.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gtazg/state.hh
${CMAKE_CURRENT_SOURCE_DIR}/transition.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gtazg/transition.hh
PARENT_SCOPE)
