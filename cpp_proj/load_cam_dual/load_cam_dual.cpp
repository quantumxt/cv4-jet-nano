// #include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

std::string gstreamer_pipeline(int cam_id, int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method, std::string outputFormat) {
  return "nvarguscamerasrc sensor_id=" + std::to_string(cam_id) + " sensor_mode=3 ! video/x-raw(memory:NVMM), format=(string)NV12, framerate=(fraction)" + std::to_string(framerate) +
    "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
    std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)" + outputFormat + " ! appsink";
}
/*
return "nvarguscamerasrc sensor_id=" + std::to_string(cam_id) + " ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
    std::to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(framerate) +
    "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
    std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)" + outputFormat + " ! appsink";
}
*/

std::string addCam(int8_t id) {
  constexpr int16_t capture_width {
    1280
  };
  constexpr int16_t capture_height {
    720
  };
  constexpr int16_t display_width {
    720
  };
  constexpr int16_t display_height {
    480
  };
  constexpr int8_t framerate {
    30
  };
  constexpr int8_t flip_method {
    6
  };
  std::string outputFormat{"BGR"}; //BGR,GRAY8
  return gstreamer_pipeline(id, capture_width,
    capture_height,
    display_width,
    display_height,
    framerate,
    flip_method,
    outputFormat);
}

int main() {

  std::string camL = addCam(0);
  std::string camR = addCam(1);

  //std::cout << "Using pipeline: \n\t" << camL << "\n";

  VideoCapture capL(camL, CAP_GSTREAMER);
  VideoCapture capR(camR, CAP_GSTREAMER);

  if (!capL.isOpened()) {
    std::cout << "Failed to open camera." << std::endl;
    return (-1);
  }

  namedWindow("Cam", WINDOW_AUTOSIZE);

  Mat imgL;
  Mat imgR;


  std::cout << "Hit ESC to exit" << "\n";
  while (true) {
    if (!capL.read(imgL) || !capR.read(imgR)) {
      std::cout << "Capture read error" << std::endl;
      break;
    }
    hconcat(imgL, imgR, imgL); //Syntax-> hconcat(source1,source2,destination);
    imshow("Cam", imgL);

    int keycode = waitKey(30) & 0xff;
    if (keycode == 27) break;
  }

  capL.release();
  capR.release();
  destroyAllWindows();
  return 0;
}
