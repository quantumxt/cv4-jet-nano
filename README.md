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

### CUDA paths

Ensure that the CUDA compiler (nvcc) is added to `~/.bashrc`, which could be done so via the `add_cuda_path.sh` script.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x add_cuda_path.sh
./add_cuda_path.sh
```

## Camera

### Setup (CSI Camera)

Configure the camera via the `jetson-io.py` script for the Jetson Orin Nano.

```sh
sudo /opt/nvidia/jetson-io/jetson-io.py
```

The terminal show display the menu. Select `Configure Jetson 24pin CSI Connector`.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                    Select one of the following:                    |
 |                                                                    |
 |                   Configure Jetson 40pin Header                    |
 |                Configure Jetson 24pin CSI Connector                |
 |                  Configure Jetson M.2 Key E Slot                   |
 |                                Exit                                |
 |                                                                    |
 |====================================================================|
```

Select `Configure for compatible hardware`.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                      3.3V (  1) .. (  2) i2c3                      |
 |                      i2c3 (  3) .. (  4) GND                       |
 |                       GND (  7) .. (  8) NA                        |
 |                        NA (  9) .. ( 10) GND                       |
 |                       GND ( 13) .. ( 14) NA                        |
 |                        NA ( 15) .. ( 16) GND                       |
 |                       GND ( 19) .. ( 20) NA                        |
 |                        NA ( 21) .. ( 22) GND                       |
 |                       GND ( 23) .. ( 24) GND                       |
 |                                                                    |
 |                                                                    |
 |                    Jetson 24pin CSI Connector:                     |
 |                                                                    |
 |                 Configure for compatible hardware                  |
 |                                Back                                |
 |====================================================================|
```

After that, select the camera model that you would be using.

> For example, if you are using a single IMX219 camera, select the `IMX219-A` option, which configures CAM0 for `IMX219-A` camera.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                Select one of the following options:                |
 |                                                                    |
 |                         Camera IMX219 Dual                         |
 |                          Camera IMX219-A                           |
 |                    Camera IMX219-A and IMX477-C                    |
 |                          Camera IMX219-C                           |
 |                         Camera IMX477 Dual                         |
 |                     Camera IMX477 Dual 4 lane                      |
 |                          Camera IMX477-A                           |
 |                    Camera IMX477-A and IMX219-C                    |
 |                          Camera IMX477-C                           |
 |                                                                    |
 |                                Back                                |
 |                                                                    |
 |                                                                    |
 |====================================================================|
```

Select `Save pin changes`.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                      3.3V (  1) .. (  2) i2c3                      |
 |                      i2c3 (  3) .. (  4) GND                       |
 |                       GND (  7) .. (  8) NA                        |
 |                        NA (  9) .. ( 10) GND                       |
 |                       GND ( 13) .. ( 14) NA                        |
 |                        NA ( 15) .. ( 16) GND                       |
 |                       GND ( 19) .. ( 20) NA                        |
 |                        NA ( 21) .. ( 22) GND                       |
 |                       GND ( 23) .. ( 24) GND                       |
 |                                                                    |
 |                                                                    |
 |                    Jetson 24pin CSI Connector:                     |
 |                                                                    |
 |                          Save pin changes                          |
 |                        Discard pin changes                         |
 |====================================================================|
```

Reboot the Jetson to use the camera!

**References**

- [https://forums.developer.nvidia.com/t/help-with-imx219-cameras/290628](https://forums.developer.nvidia.com/t/help-with-imx219-cameras/290628)
- [https://forums.developer.nvidia.com/t/no-cameras-available/320541/2](https://forums.developer.nvidia.com/t/no-cameras-available/320541/2)
- [https://docs.arducam.com/Nvidia-Jetson-Camera/Application-note/Jetson-io/](https://docs.arducam.com/Nvidia-Jetson-Camera/Application-note/Jetson-io/)

### First image

Use the `nvgstcapture-1.0` command to check whether the camera is working.

```sh
nvgstcapture-1.0
nvgstcapture-1.0 --orientation 2	# Rotate image output by 180 degrees
```

**References**

- [https://developer.nvidia.com/embedded/learn/tutorials/first-picture-csi-usb-camera](https://developer.nvidia.com/embedded/learn/tutorials/first-picture-csi-usb-camera)


## Tools

### Firefox

Install Firefox browser via `flatpak`, as there may be issues installing via snap.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x install_firefox_flatpak.sh
./install_firefox_flatpak.sh
```

 **References**
 - [https://jetsonhacks.com/2025/07/12/why-chromium-suddenly-broke-on-jetson-orin-and-how-to-bring-it-back/](https://jetsonhacks.com/2025/07/12/why-chromium-suddenly-broke-on-jetson-orin-and-how-to-bring-it-back/)
 - [https://forums.developer.nvidia.com/t/neither-chromium-nor-firefox-work-with-my-jetson-orin-nano/338669](https://forums.developer.nvidia.com/t/neither-chromium-nor-firefox-work-with-my-jetson-orin-nano/338669)
 
### Docker

Access docker without running `sudo`, by adding user to the `docker` group.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x add_docker_group.sh
./add_docker_group.sh
``` 

## Camera


## C++ project
More information could be found [here](cpp_proj/README.md).

## Archive (Jetson Nano B01 / Jetson Xavier NX)

### Camera

Ensure that the cameras are attached via the CSI before running the camera-related script/program.

#### Quickstart
```
$ gst-launch-1.0 nvarguscamerasrc sensor_mode=0 ! 'video/x-raw(memory:NVMM),width=3820, height=2464, framerate=21/1, format=NV12' ! nvvidconv flip-method=0 ! 'video/x-raw,width=960, height=616' ! nvvidconv ! nvegltransform ! nveglglessink -e
```

#### Testing

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

```sh
cd ~/cv4-jet-nano/scripts/archive
sudo chmod +x makeCUDAsample.sh
./makeCUDAsample.sh
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

