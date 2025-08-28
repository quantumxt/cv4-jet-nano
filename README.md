# Eye(s) of the Jetson Nano

<a href="LICENSE" ><img src="https://img.shields.io/github/license/quantumxt/cv4-jet-nano?style=for-the-badge"/></a>

Exploring OpenCV 4.3.0 & AI with camera(s) & C++ on the Jetson Nano B01. This serves as a documentation & reference for the various resources available for the Jetson Nano, Jetson Xavier NX & Jetson Orin Nano development board.

## Organisation
The directory is split into 3 sections:
- **AI_ML:** AI & Machine Learning related project(s)
- **cpp_proj:** C++ related projects
- **scripts:** Installation & setup scripts. Scripts for the older Jetson Nano and Jetson Xavier NX would be shifted to `archive` directory inside.

### Nvidia lib location
- libargus/Argus (For image processing): `/usr/src/jetson_multimedia_api/argus/`
- Multimedia API: `/usr/src/jetson_multimedia_api/`
- VisionWorks: `/usr/share/visionworks/sources` [Only for `Jetson Nano`/`Jetson Xavier NX`]

## Prerequisite

### Camera(s)

Ensure that the cameras are attached via the CSI before running the camera-related script/program.

### CUDA paths

Ensure that the CUDA compiler (nvcc) is added to `~/.bashrc`, which could be done so via the `add_cuda_path.sh` script.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x add_cuda_path.sh
./add_cuda_path.sh
```

## Camera

### Quickstart
```
$ gst-launch-1.0 nvarguscamerasrc sensor_mode=0 ! 'video/x-raw(memory:NVMM),width=3820, height=2464, framerate=21/1, format=NV12' ! nvvidconv flip-method=0 ! 'video/x-raw,width=960, height=616' ! nvvidconv ! nvegltransform ! nveglglessink -e
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


## C++ project
More information could be found [here](cpp_proj/README.md).

## Archive (Jetson Nano B01 / Jetson Xavier NX)

### Heatsink fan (Jetson Nano B01 ONLY)

> *Note*: Only applicable if an external heatsink fan is attached to the Jetson Nano.

Run `runFan.sh` to turn on the heatsink fan.
```
cd ~/cv4-jet-nano/scripts/archive/jetson_nano
sudo chmod +x runFan.sh
./runFan.sh
```

### CUDA Sample

The CUDA samples have to be compiled, run `makeCUDAsample.sh` to compile the samples.
> The samples would be copied and compiled at ~/cuda_samples.
```
$ ./makeCUDAsample.sh
```

### Custom OpenCV Installation
> **Note:** Compile the OpenCV source before installing ROS!

> OpenCV **4.1.1** is installed in Jetson Nano by default, but without CUDA support. Run `installOpenCV.sh` to compile & install OpenCV **4.3.0** from source. 

> Install the source at `/usr/local`.

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x installOpenCV.sh
./installOpenCV.sh
```

### ROS Melodic Installation

Run `getROS_melodic.sh` to install ROS Melodic.

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x getROS_melodic.sh
./getROS_melodic.sh
```

### OpenVSLAM Installation

> Ensure that OpenCV is installed before installing openVSLAM.
Run `getOpenVSLAM.sh` to install OpenVSLAM.

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x getOpenVSLAM.sh
./getOpenVSLAM.sh
```

# License
Licensed under the [MIT License](./LICENSE).

