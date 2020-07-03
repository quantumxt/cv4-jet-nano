# Eye(s) of the Jetson Nano

<a href="LICENSE" ><img src="https://img.shields.io/github/license/1487quantum/cv4-jet-nano?style=for-the-badge"/></a>

Exploring OpenCV 4.3.0 & AI with camera(s) & C++ on the Jetson Nano B01.

> **Note:** Compile the OpenCV source before installing ROS!

## Organisation
The directory is split into 3 sections:
- **AI_ML:** AI & Machine Learning related project(s)
- **cpp_proj:** C++ related projects
- **scripts:** Installation & setup scripts

## Camera quick test
```
$ gst-launch-1.0 nvarguscamerasrc sensor_mode=0 ! 'video/x-raw(memory:NVMM),width=3820, height=2464, framerate=21/1, format=NV12' ! nvvidconv flip-method=0 ! 'video/x-raw,width=960, height=616' ! nvvidconv ! nvegltransform ! nveglglessink -e
```

## Nvidia lib location
- libargus/Argus (For image processing): `/usr/src/jetson_multimedia_api/argus/`
- Multimedia API: `/usr/src/jetson_multimedia_api/`
- VisionWorks: `/usr/share/visionworks/sources`

## Scripts
> Python & Shell Scripts are found in the `scripts` directory.

Update the script(s) permissions before running.
```
$ sudo chmod +x 
```
**Note: Ensure that the CUDA compiler (nvcc) is added to ~/.bashrc**
```
$ echo '# Append CUDA tools path' >> ~/.bashrc
$ echo 'export PATH=/usr/local/cuda-10.2/bin${PATH:+:${PATH}}'>> ~/.bashrc
$ echo 'export LD_LIBRARY_PATH=/usr/local/cuda-10.2/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}'>> ~/.bashrc
$ echo 'export CUDA_INCLUDE_DIRS=/usr/local/cuda-10.2/include'>> ~/.bashrc
$ echo 'export CUDA_ROOT_ENV=/usr/local/cuda-10.2'>> ~/.bashrc
```

### CUDA Sample

The CUDA samples have to be compiled, run `makeCUDAsample.sh` to compile the samples.
> The samples would be copied and compiled at ~/cuda_samples.
```
$ ./makeCUDAsample.sh
```

### Heatsink fan
> Only applicable if an external heatsink fan is attached to the Jetson Nano.

Run `runFan.sh` to turn on the heatsink fan.
```
$ ./runFan.sh
```


### Testing the camera(s)

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

### OpenCV Installation
> OpenCV **4.1.1** is installed in Jetson Nano by default, but without CUDA support. Run `buildOpenCV.sh` to compile & install OpenCV **4.3.0** from source. 

> Source directory is located at `~/`, installation directory is located at `/usr/local/opencv`.
```
$ ./buildOpenCV.sh
```

### ROS Melodic Installation

Run `getROS_melodic.sh` to install ROS Melodic.
```
$ ./getROS_melodic.sh
```


### OpenVSLAM Installation

> Ensure that OpenCV is installed before installing openVSLAM.
Run `getOpenVSLAM.sh` to install OpenVSLAM.
```
$ ./getOpenVSLAM.sh
```

## Compiling the C++ project(s)
More information could be found [here](cpp_proj/README.md).

# License
Licensed under the [MIT License](./LICENSE).

