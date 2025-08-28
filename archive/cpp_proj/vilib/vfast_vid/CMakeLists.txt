cmake_minimum_required(VERSION 3.10)

# set the project name and version num
project (vfast_vid)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CUDAToolkit_ROOT ${CUDA_ROOT_ENV})


set(CMAKE_THREAD_LIBS_INIT "-lpthread")		#Add thread
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find OpenCV
find_package(OpenCV REQUIRED)
# Add vilib lib
add_library(vilib STATIC IMPORTED)
set_property(TARGET vilib PROPERTY IMPORTED_LOCATION /usr/local/vilib/lib/libvilib.so)
find_library( vilib libvilib )
# Add all the header dir
include_directories("${CUDA_INCLUDE_DIRS}" "/usr/local/vilib/include" "/usr/local/include/eigen3/")

add_dependencies(vilib vilib_a)
#Add executable
add_executable(vfast_vid vfast_vid.cpp)

# Link libraries
target_link_libraries(vfast_vid ${OpenCV_LIBS} vilib )

