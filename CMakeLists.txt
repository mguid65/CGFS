cmake_minimum_required(VERSION 3.16)
project(CGFS LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/utilities.cmake)

enable_warnings()

option(COVERAGE "Enable code coverage flags" Off)
option(SANITIZE "Enable sanitizer flags" Off)

option(CGFS_BUILD_SAMPLE "Enable building of sample" On)
option(CGFS_BUILD_TESTS "Enable building of tests" On)

if (COVERAGE)
    enable_coverage()
endif ()

if (SANITIZE)
    enable_sanitizer()
endif ()

set(CGFS_HEADERS
        include/CGFS/Canvas.hpp
)

find_package(fmt REQUIRED)

add_library(cgfs INTERFACE)
target_include_directories(cgfs INTERFACE include)
target_sources(cgfs INTERFACE ${CGFS_HEADERS})
target_link_libraries(cgfs INTERFACE fmt::fmt)
set_target_properties(cgfs PROPERTIES LINKER_LANGUAGE CXX)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS cgfs DESTINATION lib)

if (CGFS_BUILD_SAMPLE)
    add_executable(sample)
    target_sources(sample PRIVATE source/sample/main.cpp)
    target_link_libraries(sample PRIVATE cgfs)
endif ()

if (CGFS_BUILD_TESTS)
    include(CTest)
    enable_testing()

    if (CGFS_BUILD_SAMPLE)
        add_test(NAME sample
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND sample)
    endif ()

    find_package(Catch2 COMPONENTS Catch2WithMain)

    if (Catch2_FOUND)
        add_subdirectory(test)
    endif ()
endif ()
