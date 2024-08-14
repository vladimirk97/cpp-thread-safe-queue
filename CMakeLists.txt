cmake_minimum_required(VERSION 3.22.1...3.30.2)

project(thread_safe_queue
    VERSION 0.0.1
    DESCRIPTION "Thread safe queue"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/UpdateSubmodules.cmake)

include(CTest)
#
add_library(${PROJECT_NAME} INTERFACE)

# for CMAKE_INSTALL_INCLUDEDIR definition
include(GNUInstallDirs)

target_include_directories(${PROJECT_NAME}
    # PRIVATE
        # where the library will look for its internal headers
        
    INTERFACE
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external project will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(public_headers
    include/thread_safe_queue.hpp
)

if(DEFINED CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(
        STATUS
        "CMAKE_INSTALL_PREFIX is not set\n"
        "Default value: ${CMAKE_INSTALL_PREFIX}\n"
        "Will set it to ${CMAKE_SOURCE_DIR}/install"
    )
    set(CMAKE_INSTALL_PREFIX
        "${CMAKE_SOURCE_DIR}/install"
        CACHE PATH "Where the library will be installed to" FORCE
    )
else()
    message(
        STATUS
        "CMAKE_INSTALL_PREFIX was already set\n"
        "Current value: ${CMAKE_INSTALL_PREFIX}"
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Targets"
    # these get default values from GNUInstallDirs, no need to set them
    #RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    #ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    # except for public headers, as we want them to be inside a library folder
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} # include/SomeProject
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
)

# generate and install export file
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${namespace}::
    DESTINATION cmake
)
#

add_subdirectory(external)
# add_subdirectory(include)
add_subdirectory(test)



