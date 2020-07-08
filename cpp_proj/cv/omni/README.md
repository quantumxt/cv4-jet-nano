# Omnidirectional Camera Calibration & Rectification

> For fisheye images that has a FOV >= 180 degrees. [[Ref]](https://stackoverflow.com/a/49230413)

## Compilation

Refer to the README in the cpp_project directory.

## Usage
### omni_calib

Performs camera calibration with the provided imagelist file, which uses the `xml` format. Ensure that the full image path is entered instead of the relatie path.

```bash
$ ./omni_calib [IMG_LIST]  [CHECKBOARD_HORIZONTAL_POINTS]   [CHECKBOARD_VERTICAL_POINTS]  [SQUARE_WIDTH (mm)]
```
- **IMG_LIST**: List of images to be used for calibration. (A sample could be found in the `sample` directory.)
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

## References
- https://stackoverflow.com/questions/48990136/opencv-undistorts-only-a-central-part-of-fisheye-image?
- https://stackoverflow.com/questions/34316306/opencv-fisheye-calibration-cuts-too-much-of-the-resulting-image
- https://stackoverflow.com/questions/44407690/is-is-possible-to-calibrate-fish-eye-camera-using-standard-opencv
