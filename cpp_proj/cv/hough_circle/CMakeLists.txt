cmake_minimum_required(VERSION 3.5)

# set the project name and version num
project (hough)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

#Add executable
add_executable(hough hough.cpp)
target_link_libraries(hough ${OpenCV_LIBS})
