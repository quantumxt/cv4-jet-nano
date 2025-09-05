# Vision with Jetson Development Boards

<a href="LICENSE" ><img src="https://img.shields.io/github/license/quantumxt/cv4-jet-nano?style=for-the-badge"/></a>

This serves as a documentation & reference for the various resources available for the Jetson Orin Nano, Jetson Xavier NX & Jetson Nano development board with regards to Vision and AI based projects.

> The documentation/resources related to the older Jetson Nano (B01) / Jetson Xavier NX development boards would be shifted to the `archive` directory & the README [here](./archive/README_ARCHIVE.md).

> Documentation is tested on Jetpack 6.2 (`R36 (release), REVISION: 4.4`), you could check the system version via `cat /etc/nv_tegra_release`.

## Organisation
The directory is organised as follows:
- **AI_ML:** AI & Machine Learning related project(s)
- **cpp_proj:** C++ related projects
- **scripts:** Installation & setup scripts. Scripts for the older Jetson Nano and Jetson Xavier NX would be shifted to `archive` directory inside.

### Nvidia lib location
- libargus/Argus (For image processing): `/usr/src/jetson_multimedia_api/argus/`
- Multimedia API: `/usr/src/jetson_multimedia_api/`

## Prerequisite

### pip

Install python package manager `pip`.

```sh
sudo apt install python3-pip
```

### CUDA paths

Ensure that the CUDA compiler (nvcc) is added to `~/.bashrc`, which could be done so via the `add_cuda_path.sh` script.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x add_cuda_path.sh
./add_cuda_path.sh
```

#### `jetson-containers` (For Language Models)

The `jetson-containers` would be used to run the various types of language models, such as LLM, VLM, etc.

```sh
git clone https://github.com/dusty-nv/jetson-containers
bash jetson-containers/install.sh
```

Run a SLM model.

```sh
jetson-containers run $(autotag nano_llm) \
  python3 -m nano_llm.chat --api=mlc \
    --model princeton-nlp/Sheared-LLaMA-2.7B-ShareGPT
```

> The `dustynv/nano_llm:r36.4.0` container is `12.7GB`.

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

### jtop

Install `jtop` via `pip`.

```sh
sudo pip install -U jetson-stats
```

Reboot the jetson to use `jtop`.

```sh
jtop
```

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

## Troubleshooting

### `jetson-containers` not found

#### Check Jetpack installation

Check if `Jetpack` is installed via `dpkg -l | grep nvidia-jetpack`. If it returns nothing, it means that the `Jetpack` installation is missing.

Install `nvidia-jetpack`.

```sh
sudo apt update
sudo apt install -y nvidia-jetpack
```

After the installation, run `dpkg -l | grep nvidia-jetpack` to check the installation. It should print something similar as shown below.

```sh
ii  nvidia-jetpack                          6.2.1+b38                 arm64        NVIDIA Jetpack Meta Package
ii  nvidia-jetpack-dev                      6.2.1+b38                 arm64        NVIDIA Jetpack dev Meta Package
ii  nvidia-jetpack-runtime                  6.2.1+b38                 arm64        NVIDIA Jetpack runtime Meta Package
```

**References**

- [https://github.com/dusty-nv/jetson-containers](https://github.com/dusty-nv/jetson-containers)

### "Jetpack not installed" when using `jtop`

`jtop` might not be updated to display the latest Jetpack version, we could use a patch from [jetsonhacks](https://github.com/jetsonhacks/jetson-jtop-patch) to fix this issue for now.

```sh
git clone https://github.com/jetsonhacks/jetson-jtop-patch.git
cd jetson-jtop-patch
chmod +x apply_jtop_fix.sh
./apply_jtop_fix.sh
```

Reboot after the patch has been applied.

```sh
sudo reboot
```

The Jetpack version should be shown when running `jtop` again.

```sh
jtop
```

**References**
- Main: https://github.com/jetsonhacks/jetson-jtop-patch
- Alternative: https://github.com/cyaninfinite/jetson-jtop-patch

### Entering terminal session

To enter into terminal session inside the GUI (Gnome Desktop).

```sh
sudo init 3
```

To return to GUI:

```sh
sudo init 5
```

## Archive (Jetson Nano B01 / Jetson Xavier NX)

For documentation/testing of the Jetson Nano B01 / Jetson Xavier NX, checkout the README [here](./archive/README_ARCHIVE.md).

# License
Licensed under the [MIT License](./LICENSE).

