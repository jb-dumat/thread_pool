cmake_minimum_required(VERSION 3.7)

# Add all needed sources files
set(MAIN
    ${CMAKE_SOURCE_DIR}/main.cpp
)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(${PROJECT_NAME} ${SOURCES} ${MAIN})

# Add includes directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Link the project with needed libraries
target_link_libraries(${PROJECT_NAME} -pthread)
