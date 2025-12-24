# main executable
add_executable(mitl2gta mitl2gta.cpp io.cpp io.hpp parsing.cpp parsing.hpp)

target_compile_features(mitl2gta PRIVATE cxx_std_17)
set_target_properties(mitl2gta PROPERTIES CXX_EXTENSIONS OFF)

# Include directory
target_include_directories(mitl2gta PRIVATE "${mitl2gta_SOURCE_DIR}/include")

# Linking library
target_link_libraries(mitl2gta mitl2gta_LIB)

# Installing in bin directory
install(TARGETS mitl2gta DESTINATION "${CMAKE_INSTALL_BINDIR}")
