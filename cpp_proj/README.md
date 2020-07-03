# C++ Project directory
The directory is categorised based on the underlying libraries that are used:
- cv: Basic OpenCV examples
- vilib: OpenCV + vilib_fast
- visionworks: Nvidia VisionWorks library

> **Note:** Projects in the various directories would require relevant dependencies for it to compile, more details are available in the sections below.

## Projects
### cv
All the relevant OpenCV example are located here, ensure that OpenCV is installed beforehand.

> If OpenCV is compiled from source, ensure that it is installed into the system via `$ sudo make install`. The typical OpenCV source location would be `/usr/local` or `/usr/local/opencv`.
> For OpenCV python support, ensure that the OpenCV path is appeneded to the environment variable, for example adding the following line in `.bashrc`: `export PYTHONPATH=$PYTHONPATH:/usr/local/opencv-3.4.10/release/python_loader/`

### vilib
More information could be found [here](https://github.com/1487quantum/vilib_ros).

### VisionWorks
Ensure that the `glfw3-dev` library is installed and the NVXIO library is compiled:
**Install dependencies**
```bash
$ sudo apt install libglfw3-dev
```
**Build NVXIO**
```
$ cd /usr/share/visionworks/sources/
$ sudo make
```
After that, proceed on to compile the projects.

## Compiling the C++ project(s)
Each project would be created in a standalone directory, and CMake would be used to compile the cpp files. 
> Ensure that the cpp files & CMakeLists.txt are present in the parent directory before running CMake!

### Via Script
Run the script of the target directory:
```
$ ./compile.sh [target_dir]
```
For example, `$ ./compile.sh cv/cam_fps`

> **Note:** Specify the directory without the backslash behind!

### Manually
Create a `build` directory in the parent directory & enter the directory.
```bash
$ cd parent_dir
$ mkdir build && cd build
```
After that, run `cmake` to generate the Makefiles in the current `build` directory:
```bash
$ cmake -G 'Unix Makefiles' ..
```
After that, compile the files:
```bash
$ make -j4
