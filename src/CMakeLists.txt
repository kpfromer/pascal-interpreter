# Include source files
file(GLOB SRC_FILES
        "./*.h"
        "./*.cpp"
        )
add_executable (PascalInterpreter ${SRC_FILES})