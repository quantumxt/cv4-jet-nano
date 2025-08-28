# Omnidirectional Camera Calibration & Rectification

## Compilation

Refer to the README in the cpp_project directory.

## Usage
### omni_calib [Mono]

Performs camera calibration with the provided imagelist file, which uses the `xml` format. Ensure that the full image path is entered instead of the relatie path.

```bash
$ ./omni_calib [IMG_LIST]  [CHECKBOARD_HORIZONTAL_POINTS]   [CHECKBOARD_VERTICAL_POINTS]  [SQUARE_WIDTH (mm)]
```
- **IMG_LIST**: List of images to be used for calibration. (A sample could be found in the `sample` directory.)
- **CHECKBOARD_HORIZONTAL_POINTS**: Number of horizontal points on checker, count by edges of square. 
- **CHECKBOARD_VERTICAL_POINTS**: Number of vertical points on checker, count by edges of square. 
- **SQUARE_WIDTH**: Size of checkerboard square, measured in millimetres (mm).

> **Note:** Ensure that the both checkerboard horizontal & vertical points are more than 2, else the calibration wouldn't work!

### omni_calib_stereo

Performs camera calibration with the provided imagelist file, which uses the `xml` format. Ensure that the full image path is entered instead of the relatie path.

```bash
$ ./omni_calib_stereo [IMG_LIST_LEFT]  [IMG_LIST_RIGHT]  [CHECKBOARD_HORIZONTAL_POINTS]   [CHECKBOARD_VERTICAL_POINTS]  [SQUARE_WIDTH (mm)]
```
- **IMG_LIST_LEFT**: List of *left* images to be used for calibration. (A sample could be found in the `sample` directory.)
- **IMG_LIST_RIGHT**: List of *right* images to be used for calibration. (A sample could be found in the `sample` directory.)
- **CHECKBOARD_HORIZONTAL_POINTS**: Number of horizontal points on checker, count by edges of square. 
- **CHECKBOARD_VERTICAL_POINTS**: Number of vertical points on checker, count by edges of square. 
- **SQUARE_WIDTH**: Size of checkerboard square, measured in millimetres (mm).

> **Note:** Ensure that the both checkerboard horizontal & vertical points are more than 2, else the calibration wouldn't work!

### omni_rectify

Performs image rectification on target image.
```bash
$ ./omni_rectify [CALIBRATION_FILE]  [IMG_TO_DISTORT]  [ZOOM_OUT_LEVEL]
```
- **CALIBRATION_FILE**: Calibration file created by `omni_calib`, uses the `xml` format. (A sample could be found in the `sample` directory.)
- **IMG_TO_DISTORT**: Target image to be rectified using the calibration configuration.
- **ZOOM_OUT_LEVEL**: Distance from the center of the image. Larger number corresponds to a larger FoV (Field of View). Ranges from 1.0 <-> 7.0.


## Known Issues
- Stereo calibration crashes with if the last few image path in the image list is not found.
```
terminate called after throwing an instance of 'cv::Exception'
  what():  OpenCV(3.4.10) /usr/local/opencv-3.4.10/modules/core/src/matrix_wrap.cpp:1659: error: (-215:Assertion failed) !fixedSize() in function 'release'
```

