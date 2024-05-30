cmake_minimum_required(VERSION 3.1)
project(bsr CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
    main.cpp
    task.cpp
    search.cpp
    changer.cpp
    options.cpp
)
