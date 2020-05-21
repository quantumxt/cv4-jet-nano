cmake_minimum_required(VERSION 3.5)

# set the project name and version num
project (load_cam_dual)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

#Add executable
add_executable(load_cam_dual load_cam_dual.cpp)
target_link_libraries(load_cam_dual ${OpenCV_LIBS})
