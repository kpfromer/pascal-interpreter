file(GLOB SRC_FILES
#        "./*.h"
        "../src/*.cpp"
        )

add_executable(test_fact test.cpp test_interpreter.cpp ../src/interpreter.cpp)
target_include_directories(test_fact PRIVATE ${CMAKE_SOURCE_DIR}/include)

#enable_testing()
#add_test(NAME InterpreterTest COMMAND test_fact)