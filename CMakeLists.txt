project (PascalInterpreter)

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)

include_directories(include)

add_subdirectory(src)
add_subdirectory(test)

## Include source files
#file(GLOB SRC_FILES
#    "src/*.h"
#    "src/*.cpp"
#)
#add_executable (PascalInterpreter ${SRC_FILES} src/catch2.h)

# Include Boost Library
# find_package(Boost 1.69 REQUIRED COMPONENTS filesystem system)
# include_directories(${Boost_INCLUDE_DIRS})
# target_link_libraries(JackCompiler ${Boost_LIBRARIES})