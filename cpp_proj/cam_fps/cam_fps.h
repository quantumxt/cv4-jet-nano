// #include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
//#include <numeric>

using namespace cv;

// Var
//FPS
typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> fsec;
//Display
constexpr int8_t s_mode{3};
constexpr int16_t display_width{720};
constexpr int16_t display_height{360};
constexpr int8_t framerate{30};
constexpr int8_t flip_method{6};

// Fx
// Camera
std::string gstreamer_pipeline(int8_t cam_id, int8_t s_mode, int display_width,
                               int display_height, int8_t framerate,
                               int8_t flip_method, std::string outputFormat);
std::string addCam(int8_t id);


// Draw functions
Mat drawRect(Mat img, int x, int y, int w, int h);
//x, y: starting xy position for text
Mat drawText(Mat img, int x, int y, std::string msg);
Mat addCross(Mat img, int x, int y, int m_size);

