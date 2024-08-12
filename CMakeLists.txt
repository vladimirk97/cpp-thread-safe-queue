cmake_minimum_required(VERSION 3.22.1...3.30.2)

set(PROJECT thread_safe_queue)

project(${PROJECT} 
    VERSION 0.0.1
    DESCRIPTION "Thread safe queue"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/UpdateSubmodules.cmake)

include(CTest)

add_subdirectory(external)
add_subdirectory(thread_safe_queue)



