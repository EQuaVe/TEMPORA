cmake_minimum_required(VERSION 3.15...3.27)

project(mitl2gta VERSION 1.1
                 DESCRIPTION "Translate a MITL formula to a network of GTA"
                 LANGUAGES CXX)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)

