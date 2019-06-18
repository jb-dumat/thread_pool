cmake_minimum_required(VERSION 3.7)
project(thread_pool)

# Add all needed sources files
set(SOURCES
    ${CMAKE_SOURCE_DIR}/source/Service.cpp
	${CMAKE_SOURCE_DIR}/source/ThreadPool.cpp
)

# Compiler flags
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
elseif(GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")
endif()

add_subdirectory(tests)
add_subdirectory(project)