find_package( Boost REQUIRED COMPONENTS program_options regex )
include_directories( ${Boost_INCLUDE_DIRS} )

# Include source files
file(GLOB SRC_FILES
        "./*.h"
        "./*.cpp"
        )
add_executable (PascalInterpreter ${SRC_FILES})