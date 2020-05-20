# cv4_cpp
Exploring OpenCV 4.3.0 with C++ 

## Compiling the project(s)
Each project would be created in a standalone directory, and CMake would be used to compile the cpp files. 
> Ensure that the cpp files & CMakeLists.txt are present in the parent directory before reunning CMake!

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
$ make
```

# License
Licensed under the [MIT License](./LICENSE).

