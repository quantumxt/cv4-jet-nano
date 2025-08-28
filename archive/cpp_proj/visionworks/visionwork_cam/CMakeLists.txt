cmake_minimum_required(VERSION VERSION 3.10)

project (cam)                   # set the project name
find_package(VisionWorks REQUIRED)

set(CMAKE_CXX_STANDARD 11)      # specify the C++ standard

include_directories("/usr/share/visionworks/sources/nvxio/include")
link_directories("/usr/local/cuda/targets/aarch64-linux/lib" "/usr/share/visionworks/sources/libs/aarch64/linux/release")

find_package(OpenCV 3 REQUIRED) 
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(VisionWorks REQUIRED)
include_directories( ${VisionWorks_INCLUDE_DIRS} )

add_executable(cam cam.cpp)             #Add executable

target_link_libraries(cam 
    -L"/usr/lib" 
    /usr/share/visionworks/sources/libs/aarch64/linux/release/libovx.a 
    /usr/share/visionworks/sources/3rdparty/freetype/libs/libfreetype.a 
    -L/usr/lib/aarch64-linux-gnu 
    -lEGL 
    -lXi 
    -lXxf86vm 
    -lX11 
    -lgstpbutils-1.0 
    -lgstaudio-1.0 
    -lgstvideo-1.0 
    -lgstapp-1.0 
    -lgstbase-1.0 
    -lgstreamer-1.0 
    -lgobject-2.0 
    -lglib-2.0 
    /usr/lib/aarch64-linux-gnu/tegra/libcuda.so  
    -L/usr/local/cuda-10.2/targets/aarch64-linux/lib 
    -lcudart 
    -lvisionworks 
    /usr/share/visionworks/sources/3rdparty/glfw3/libs/libglfw3.a 
    /usr/lib/aarch64-linux-gnu/tegra-egl/libGLESv2_nvidia.so.2 
    -L/usr/lib/aarch64-linux-gnu 
    -lXrandr 
    -lXi 
    -pthread ${VisionWorks_LIBRARIES})

