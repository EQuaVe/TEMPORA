# MITL2GTA library include directory
set(mitl2gta_INCLUDE_DIR "${mitl2gta_SOURCE_DIR}/include/mitl2gta")

add_library(mitl2gta_LIB STATIC)

add_subdirectory(boolean_expression)
add_subdirectory(clock)
add_subdirectory(compile)
add_subdirectory(gta_system)
add_subdirectory(interval)
add_subdirectory(memory)
add_subdirectory(mitl_formula)
add_subdirectory(parsing)
add_subdirectory(sharer_automaton)
add_subdirectory(transducer)
add_subdirectory(utils)

target_compile_features(mitl2gta_LIB PUBLIC cxx_std_17)
set_target_properties(mitl2gta_LIB PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(mitl2gta_LIB PUBLIC "${mitl2gta_SOURCE_DIR}/include")

