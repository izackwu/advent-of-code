cmake_minimum_required(VERSION 3.15)
project(HighsProject)

set(CMAKE_CXX_STANDARD 23)

# 1. Find HiGHS
find_package(highs CONFIG REQUIRED)

add_executable(main.out main.cpp)

# 2. Link HiGHS
# Note: Newer HiGHS versions export target 'highs::highs', older ones might just define libs.
if(TARGET highs::highs)
    target_link_libraries(main.out PRIVATE highs::highs)
else()
    # Fallback for some installs
    include_directories(${HIGHS_INCLUDE_DIRS})
    target_link_libraries(main.out PRIVATE ${HIGHS_LIBRARIES})
endif()
