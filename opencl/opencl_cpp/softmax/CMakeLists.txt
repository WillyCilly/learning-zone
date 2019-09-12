# minimum version
cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

#setting a project
project(mnist_test)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS} ../../common)
link_directories(${OpenCL_LIBRARY})
set(CMAKE_CXX_STANDARD 11)
message("INFO: Compiling with OpenCL " ${OpenCL_VERSION_MAJOR}.${OpenCL_VERSION_MINOR})

# making an executable as a target
add_executable(softmax.o softmax.cpp)
target_link_libraries (softmax.o ${OpenCL_LIBRARY})
