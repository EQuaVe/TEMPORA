target_sources(mitl2gta_LIB 
  PRIVATE 
  "${CMAKE_CURRENT_SOURCE_DIR}/memory_handler.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/memory_mapping.cpp"
  PUBLIC
  "${mitl2gta_INCLUDE_DIR}/memory/memory_handler.hpp"
  "${mitl2gta_INCLUDE_DIR}/memory/memory_mapping.hpp"
)
