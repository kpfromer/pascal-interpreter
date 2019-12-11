project (PascalInterpreter)

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)

## Include source files
file(GLOB SRC_FILES "src/*")

add_executable (PascalInterpreter ${SRC_FILES})