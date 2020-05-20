cmake_minimum_required(VERSION 3.5)

# set the project name and version num
project (load_img)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)

#Add executable
add_executable(load_img load_img.cpp)
target_link_libraries(load_img ${OpenCV_LIBS})
