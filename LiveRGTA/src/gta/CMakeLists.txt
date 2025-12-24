
# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(GTA_SRC
${CMAKE_CURRENT_SOURCE_DIR}/gta.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta/gta.hh
${CMAKE_CURRENT_SOURCE_DIR}/static_analysis.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta/static_analysis.hh
${CMAKE_CURRENT_SOURCE_DIR}/state.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta/state.hh
${CMAKE_CURRENT_SOURCE_DIR}/system.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta/system.hh
${CMAKE_CURRENT_SOURCE_DIR}/transition.cc
${TCHECKER_INCLUDE_DIR}/tchecker/gta/transition.hh
PARENT_SCOPE)
