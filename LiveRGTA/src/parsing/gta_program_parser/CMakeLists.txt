# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(BISON REQUIRED 3.0.4)
find_package(FLEX REQUIRED)

BISON_TARGET(gta_program_parser gta_program.yy
${CMAKE_CURRENT_BINARY_DIR}/gta_program.tab.cc
DEFINES_FILE   ${CMAKE_CURRENT_BINARY_DIR}/gta_program.tab.hh
COMPILE_FLAGS  -v)

FLEX_TARGET(gta_program_lexer gta_program.ll ${CMAKE_CURRENT_BINARY_DIR}/gta_program.lex.cc COMPILE_FLAGS -Pgpyy)

ADD_FLEX_BISON_DEPENDENCY(gta_program_lexer gta_program_parser)

# Fixing flex output (register variable class deprecated in c++11)
add_custom_command(OUTPUT gta_program.lex-fixed.cc
COMMAND sed -e "s/register//g" ${FLEX_gta_program_lexer_OUTPUTS} > gta_program.lex-fixed.cc
DEPENDS ${FLEX_gta_program_lexer_OUTPUTS})

# Build gta program parsing static object library
add_library(gta_program_parsing_static OBJECT
  ${BISON_gta_program_parser_OUTPUTS} gta_program.lex-fixed.cc gta_program.ll gta_program.yy)
set_property(TARGET gta_program_parsing_static PROPERTY CXX_STANDARD 17)
set_property(TARGET gta_program_parsing_static PROPERTY CXX_STANDARD_REQUIRED ON)

# Build gta program parsing shared object library
if(LIBTCHECKER_ENABLE_SHARED)
  add_library(gta_program_parsing_shared OBJECT
    ${BISON_gta_program_parser_OUTPUTS} gta_program.lex-fixed.cc gta_program.ll gta_program.yy)
  set_property(TARGET gta_program_parsing_shared PROPERTY CXX_STANDARD 17)
  set_property(TARGET gta_program_parsing_shared PROPERTY CXX_STANDARD_REQUIRED ON)
  if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    set_property(TARGET gta_program_parsing_shared PROPERTY
      POSITION_INDEPENDENT_CODE 1)
  endif()
endif()
