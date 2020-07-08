cmake_minimum_required(VERSION 3.5)

# set the project name and version num
project (omni)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

#Add executable
add_executable(omni_calib omni_mono_calib.cpp)
add_executable(omni_rectify omni_rectify.cpp)
target_link_libraries(omni_calib ${OpenCV_LIBS})
target_link_libraries(omni_rectify ${OpenCV_LIBS})
