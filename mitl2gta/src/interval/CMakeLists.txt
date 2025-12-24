target_sources(mitl2gta_LIB 
  PRIVATE 
  "${CMAKE_CURRENT_SOURCE_DIR}/bound.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/interval.cpp"
  PUBLIC
  "${mitl2gta_INCLUDE_DIR}/interval/bound.hpp"
  "${mitl2gta_INCLUDE_DIR}/interval/interval.hpp"
)
