#cmake ver req
cmake_minimum_required(VERSION 3.1.0)

#project name
set(project_name serializer)
project(${project_name} C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14  -Wall -Wextra -Wshadow -Wconversion -Wno-long-long -pedantic -Wno-unused-parameter")

include_directories("tests")


file(GLOB_RECURSE source_files "./*.cpp")
file(GLOB_RECURSE header_files "./*.hpp")
add_executable(${project_name} ${header_files} ${source_files})
