cmake_minimum_required(VERSION 3.0)

project(contours)

find_package( OpenCV REQUIRED)

SET( CMAKE_BUILD_TYPE "Debug" )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(contours main.cpp)
target_link_libraries( contours ${OpenCV_LIBS} )
