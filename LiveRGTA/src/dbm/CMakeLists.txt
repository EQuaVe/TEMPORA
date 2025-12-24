# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(DBM_SRC
${CMAKE_CURRENT_SOURCE_DIR}/db.cc
${CMAKE_CURRENT_SOURCE_DIR}/dbm.cc
${CMAKE_CURRENT_SOURCE_DIR}/refdbm.cc
${CMAKE_CURRENT_SOURCE_DIR}/gtadbm.cc
${TCHECKER_INCLUDE_DIR}/tchecker/dbm/db.hh
${TCHECKER_INCLUDE_DIR}/tchecker/dbm/details/db_safe.hh
${TCHECKER_INCLUDE_DIR}/tchecker/dbm/details/db_unsafe.hh
${TCHECKER_INCLUDE_DIR}/tchecker/dbm/dbm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/dbm/refdbm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/dbm/gtadbm.hh
PARENT_SCOPE)
