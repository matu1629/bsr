cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_executable(test_task
    test_task.cpp
    ../src/task.cpp
)
add_test(NAME run_test_task COMMAND test_task)

add_executable(test_options
    test_options.cpp
    ../src/options.cpp
)
add_test(NAME run_test_options COMMAND test_options)

add_executable(test_changer
    test_changer.cpp
    ../src/changer.cpp
)
add_test(NAME run_test_changer COMMAND test_changer)

add_executable(test_search
    test_search.cpp
    ../src/search.cpp
    ../src/task.cpp
    ../src/changer.cpp
)
add_test(NAME run_test_search COMMAND test_search)