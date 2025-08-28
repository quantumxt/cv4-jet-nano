#include "cam_fps.h"

std::string gstreamer_pipeline(int8_t cam_id, int8_t s_mode, int display_width,
                               int display_height, int8_t framerate,
                               int8_t flip_method, std::string outputFormat) {
  return "nvarguscamerasrc sensor_id=" + std::to_string(cam_id) +
         " sensor_mode=" + std::to_string(s_mode) +
         " ! video/x-raw(memory:NVMM), format=(string)NV12, "
         "framerate=(fraction)" +
         std::to_string(framerate) + "/1 ! nvvidconv flip-method=" +
         std::to_string(flip_method) + " ! video/x-raw, width=(int)" +
         std::to_string(display_width) + ", height=(int)" +
         std::to_string(display_height) +
         ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)" +
         outputFormat + " ! appsink";
}

std::string addCam(int8_t id) {
  std::string outputFormat{"BGR"}; // BGR,GRAY8
  return gstreamer_pipeline(id, s_mode, display_width, display_height,
                            framerate, flip_method, outputFormat);
}

Mat drawRect(Mat img, int x, int y, int w, int h) {
  cv::Rect rect(x, y, w, h);
  cv::rectangle(img, rect, cv::Scalar(0, 255, 0));
  return img;
}

// x, y: starting xy position for text
Mat drawText(Mat img, int x, int y, std::string msg) {
  Point pt_loc(x, y);
  putText(img, msg, pt_loc, FONT_HERSHEY_COMPLEX_SMALL, 0.8,
          cv::Scalar(0, 200, 250), 1, LINE_AA);
  return img;
}

Mat addCross(Mat img, int x, int y, int m_size) {
  Point pt_loc(x, y);
  drawMarker(img, pt_loc, cv::Scalar(255, 255, 255), MARKER_CROSS, m_size, 1,
             8);
  return img;
}

int main() {
  std::string camL = addCam(0);

  std::cout << "Running with OpenCV Version: " << CV_VERSION << "\n";
  // std::cout << "Using pipeline: \n\t" << camL << "\n";

  VideoCapture capL(camL, CAP_GSTREAMER);

  if (!capL.isOpened()) {
    std::cout << "Failed to open camera." << std::endl;
    return (-1);
  }

  namedWindow("IMG", WINDOW_AUTOSIZE);
  std::cout << "Hit ESC to exit"
            << "\n";

  Mat imgL;	// Camera Feed
  Rect wa; 	// Get display window size

  // Track time
  long frameCounter = 0;
  long fps = 0;
  std::time_t timeBegin = std::time(0);
  int tick = 0;

  while (true) {
    auto start = Time::now(); // Start timer
    int capr = capL.read(imgL);
    if (!capr) {
      std::cout << "Capture read error" << std::endl;
      break;
    }

    // std::cout << wa.x << ","  << wa.y << "," << wa.width << ","  << wa.height
    // << std::endl;

    // Draw a smaller rectangle overlay
    wa = getWindowImageRect("IMG"); // Get display width and height
    imgL = drawRect(imgL, wa.width * 0.25, wa.height * 0.25, wa.width * 0.5,
                    wa.height * 0.5);

    // Add marker at center
    imgL = addCross(imgL, wa.width / 2, wa.height / 2, 20);

    // Display fps
    std::string t_fps = "FPS: " + std::to_string(fps);
    imgL = drawText(imgL, 30, 30, t_fps);

    imshow("IMG", imgL);

    // FPS
    frameCounter++;

    std::time_t timeNow = std::time(0) - timeBegin;
    if (timeNow - tick >= 1) {
      tick++;
      fps = frameCounter;
      // std::cout << "Frames per second: " << frameCounter << std::endl;
      frameCounter = 0;
    }

    // ESC to escape
    int keycode = waitKey(30) & 0xff;
    if (keycode == 27)
      break;
  }

  capL.release();

  destroyAllWindows();
  return 0;
}
