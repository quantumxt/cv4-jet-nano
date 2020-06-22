cmake_minimum_required(VERSION VERSION 3.10)

project (cam)                   # set the project name
find_package(VisionWorks REQUIRED)

set(CMAKE_CXX_STANDARD 11)      # specify the C++ standard

# Add all the header dir
#include_directories("/usr/share/visionworks/sources/nvxio/include")
#link_directories("/usr/local/cuda/targets/aarch64-linux/lib" "/usr/share/visionworks/sources/libs/aarch64/linux/release")

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-lvisionworks -lovx -lnvx -lpthread")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGE} "-lgstpbutils-1.0 -lgstapp-1.0 -lgstbase-1.0 -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGE} "/usr/share/visionworks/sources/3rdparty/glfw3/libs/libglfw3.a")
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGE} "-lXrandr -lXi -lXxf86vm -lX11 -lGL")


include_directories( 
    ${VisionWorks_INCLUDE_DIRS}
    /usr/local/cuda/include
   # -I/usr/local/cuda-10.2/targets/aarch64-linux/include

   # /usr/include
   # /usr/include/VX
   # /usr/share/visionworks/sources/nvxio/include

    #/usr/local/cuda-10.2/targets/aarch64-linux/include 
    
    /usr/local/cuda-10.2/targets/aarch64-linux/include  
    /usr/share/visionworks/sources/nvxio/include 
    /usr/share/visionworks/sources/nvxio/src/ 
    /usr/share/visionworks/sources/nvxio/src/NVX/ 
    /usr/share/visionworks/sources/3rdparty/opengl 
    /usr/share/visionworks/sources/3rdparty/glfw3/include 
    /usr/share/visionworks/sources/3rdparty/freetype/include 
    /usr/share/visionworks/sources/3rdparty/eigen

     )

#link_directories(/usr/share/visionworks/sources/libs/aarch64/linux/release)

add_executable(cam cam.cpp)             #Add executable

target_link_libraries(
    cam 
    
    -L"/usr/lib" 
    /usr/share/visionworks/sources/libs/aarch64/linux/release/libovx.a 
    /usr/share/visionworks/sources/3rdparty/freetype/libs/libfreetype.a 
    /usr/share/visionworks/sources/3rdparty/glfw3/libs/libglfw3.a 
    /usr/lib/aarch64-linux-gnu/tegra-egl/libGLESv2_nvidia.so.2 
    -L/usr/lib/aarch64-linux-gnu 
    -lEGL 
    -lXrandr 
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
    -L/usr/local/cuda-10.2/targets/aarch64-linux/lib 
    -lcudart 
    -lvisionworks 
    /usr/share/visionworks/sources/3rdparty/glfw3/libs/libglfw3.a 
    /usr/lib/aarch64-linux-gnu/tegra-egl/libGLESv2_nvidia.so.2 
    -L/usr/lib/aarch64-linux-gnu 
    -lEGL 
    -lXrandr 
    -lXi 
    -lXxf86vm 
    -lX11 
    -lrt 
    -pthread
    )