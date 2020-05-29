# Eye(s) of the Jetson Nano

<a href="LICENSE" ><img src="https://img.shields.io/github/license/1487quantum/cv4-jet-nano?style=for-the-badge"/></a>

Exploring OpenCV 4.3.0 & AI with camera(s) & C++ on the Jetson Nano B01.

## Testing the camera(s)
> Scripts are found in the `scripts` directory.

Make the `runCam.sh` script executable:
```bash
$ chmod + x runCam.sh
```
Test the [camera](https://elinux.org/Jetson_Nano#Cameras) connected via `runCam.sh`:
```bash
$ runCam.sh
```
To test 2 cameras, a `-d` flag would be added:
```bash
$ runCam.sh -d
```
> **[Additional information on Gstreamer](https://developer.ridgerun.com/wiki/index.php?title=Jetson_Nano/Gstreamer/Example_Pipelines/Capture_Display)**

## Compiling the project(s)
> Projects are found in the cpp_proj directory

Each project would be created in a standalone directory, and CMake would be used to compile the cpp files. 
> Ensure that the cpp files & CMakeLists.txt are present in the parent directory before reunning CMake!

### Script
Update the script (inside the `cpp_proj` dir) permissions before running.
```
$ sudo chmod +x 
```
After that, run the script of the target directory:
```
$ ./compile.sh [target_dir]
```
> For example, `$ ./compile.sh cam_fps`

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
```

# License
Licensed under the [MIT License](./LICENSE).

