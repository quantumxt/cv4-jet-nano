# Archive (Jetson Nano B01 / Jetson Xavier NX)

The instructions for testing or installing specific packages for the Jetson Nano B01 / Jetson Xavier NX development boards have been shifted to this README_ARCHIVE file, instead of the main README file.

## Camera

Ensure that the cameras are attached via the CSI before running the camera-related script/program.

### Quickstart
```
$ gst-launch-1.0 nvarguscamerasrc sensor_mode=0 ! 'video/x-raw(memory:NVMM),width=3820, height=2464, framerate=21/1, format=NV12' ! nvvidconv flip-method=0 ! 'video/x-raw,width=960, height=616' ! nvvidconv ! nvegltransform ! nveglglessink -e
```

### Testing

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


## Heatsink fan (Jetson Nano B01 ONLY)

> *Note*: Only applicable if an external heatsink fan is attached to the Jetson Nano.

Run `runFan.sh` to turn on the heatsink fan.
```
cd ~/cv4-jet-nano/scripts/archive/jetson_nano
sudo chmod +x runFan.sh
./runFan.sh
```

## CUDA Sample

The CUDA samples have to be compiled, run `makeCUDAsample.sh` to compile the samples.

> The samples would be copied and compiled at ~/cuda_samples.

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x makeCUDAsample.sh
./makeCUDAsample.sh
```

## Installation
### OpenCV with CUDA support
> **Note:** Compile the OpenCV source before installing ROS!

> OpenCV **4.1.1** is installed in Jetson Nano by default, but without CUDA support. Run `installOpenCV.sh` to compile & install OpenCV **4.3.0** from source. 

> Install the source at `/usr/local`.

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x installOpenCV.sh
./installOpenCV.sh
```

### ROS Melodic

Run `getROS_melodic.sh` to install ROS Melodic.

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x getROS_melodic.sh
./getROS_melodic.sh
```

### OpenVSLAM

> Ensure that OpenCV is installed before installing openVSLAM.
Run `getOpenVSLAM.sh` to install OpenVSLAM.

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x getOpenVSLAM.sh
./getOpenVSLAM.sh
```

## C++ project
More information could be found [here](cpp_proj/README.md).
