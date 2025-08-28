cmake_minimum_required(VERSION 3.5)

# set the project name and version num
project (cam_fps)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

#Add executable
add_executable(cam_fps cam_fps.cpp)
target_link_libraries(cam_fps ${OpenCV_LIBS})
