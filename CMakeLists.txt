cmake_minimum_required(VERSION 3.16)

project(mirage)

# define global output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/../../lib)

# add main frame projects
add_subdirectory(project/mango)

message(EXECUTABLE_OUTPUT_PATH ="${EXECUTABLE_OUTPUT_PATH}")
message(LIBRARY_OUTPUT_PATH    ="${EXECUTABLE_OUTPUT_PATH}")