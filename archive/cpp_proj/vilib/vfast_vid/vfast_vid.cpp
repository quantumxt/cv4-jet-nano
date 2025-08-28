#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudev/common.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

#include <thread>
#include <future>
#include <mutex>

#include "vilib/cuda_common.h"
#include "vilib/preprocess/pyramid.h"
#include "vilib/storage/pyramid_pool.h"
#include "vilib/feature_detection/fast/fast_gpu.h"
#include "vilib/feature_detection/detector_base.h"
#include "vilib/feature_detection/detector_base_gpu.h"
#include "vilib/config.h"
#include "vilib/timer.h"
#include "vilib/statistics.h"

using namespace cv;
using namespace vilib;

std::shared_ptr<vilib::DetectorBaseGPU> detector_gpu_;

// Frame preprocessing
#define PYRAMID_LEVELS 1
#define PYRAMID_MIN_LEVEL 0
#define PYRAMID_MAX_LEVEL PYRAMID_LEVELS

// FAST detector parameters
#define FAST_EPSILON (10.0f)
#define FAST_MIN_ARC_LENGTH 10
// Remark: the Rosten CPU version only works with
//         SUM_OF_ABS_DIFF_ON_ARC and MAX_THRESHOLD
#define FAST_SCORE SUM_OF_ABS_DIFF_ON_ARC

// NMS parameters
#define HORIZONTAL_BORDER 0
#define VERTICAL_BORDER 0
#define CELL_SIZE_WIDTH 32
#define CELL_SIZE_HEIGHT 32

Mat imgL; // Colored Feed
Mat imgGray; // Grayscale version
int capr{ 0 }; //VidCapture
bool startP{ false };
bool vidEnd{ false };
std::unordered_map<int, int> pts; //Feature points detected

//Threading
std::mutex m; //you can use std::lock_guard if you want to be exception safe

// Track time
long frameCounter{ 0 };
long fps{ 0 };
std::time_t timeBegin{ std::time(0) };
int tick{ 0 };

// === FEATURE DETECTOR ===

//FASt corner detector fx, return all the points detected
std::unordered_map<int, int> fDetector(Mat img)
{
    //For pyramid storage
    int image_width_ = img.cols;
    int image_height_ = img.rows;

    //std::cout << "Detector (GPU) reset" << std::endl;
    detector_gpu_.reset(new FASTGPU(image_width_,
        image_height_,
        CELL_SIZE_WIDTH,
        CELL_SIZE_HEIGHT,
        PYRAMID_MIN_LEVEL,
        PYRAMID_MAX_LEVEL,
        HORIZONTAL_BORDER,
        VERTICAL_BORDER,
        FAST_EPSILON,
        FAST_MIN_ARC_LENGTH,
        FAST_SCORE));

    // Initialize the pyramid pool
    PyramidPool::init(1,
        image_width_,
        image_height_,
        1, // grayscale
        PYRAMID_LEVELS,
        IMAGE_PYRAMID_MEMORY_TYPE);

    // Create a Frame (image upload, pyramid)
    std::shared_ptr<Frame> frame0(new Frame(img, 0, PYRAMID_LEVELS));
    // Reset detector's grid
    // Note: this step could be actually avoided with custom processing
    detector_gpu_->reset();
    // Do the detection
    detector_gpu_->detect(frame0->pyramid_);

    // Display results
    //std::cout << "Display" << std::endl;

    auto& points_gpu = detector_gpu_->getPoints();
    auto& points_gpu_grid = detector_gpu_->getGrid();

    std::unordered_map<int, int> points_combined;
    points_combined.reserve(points_gpu.size());

    int qqq = 0;
    for (auto it = points_gpu.begin(); it != points_gpu.end(); ++it) {
        int key = ((int)it->x_) | (((int)it->y_) << 16);

        if (key) {
            //std::cout << "#" << qqq << " , key: " << key << std::endl;
            points_combined.emplace(key, 3);
        }
        else {
            points_combined.emplace(key, 1);
        }
        qqq++;
    }

    //std::cout<<"All points: "<<points_gpu.size()<<std::endl;
    // std::cout<<"Interest points: "<<points_combined.size()<<std::endl;

    return points_combined;
}

// === GRAPHICS ===

// x, y: starting xy position for text
Mat drawText(Mat img, int x, int y, std::string msg)
{
    putText(img, msg, cv::Point(x, y), FONT_HERSHEY_COMPLEX_SMALL, 0.8,
        cv::Scalar(255, 255, 255), 1, LINE_AA);
    return img;
}

//Draw circle
Mat dCircle(Mat img, int x, int y)
{
    int thickness = 1;
    cv::circle(img,
        cv::Point(x, y),
        1 * 3 * 1024,
        cv::Scalar(0, 255, 255),
        thickness,
        8,
        10);
    return img;
}


//Draw text & detected features on img
Mat processImg(Mat img, std::unordered_map<int, int> pts, int fps)
{

    // draw circles for the identified keypoints
    for (auto it = pts.begin(); it != pts.end(); ++it) {
        int x = (it->first & 0xFFFF) * 1024;
        int y = ((it->first >> 16) & 0xFFFF) * 1024;
        //std::cout << "x: " << x << " , y: " << y << std::endl;
        cv::Scalar color; // B,G,R
        int thickness = 1;
        if (it->second == 3) {
            img = dCircle(img, x, y);
        }
    }

    //Draw text on img
    std::string tPoints = "Corners: " + std::to_string(pts.size());
    img = drawText(img, 30, 30, tPoints);

    // Display fps
    std::string t_fps = "FPS: " + std::to_string(fps);
    img = drawText(img, 30, 50, t_fps);

	std::cout << tPoints <<", " <<t_fps<< "\t=== ";	//Detector, then display threads running

    return img;
}

// === THREADS ===

// Main detector thread (MDT)
void runProcess(VideoCapture capL)
{
    while (true&&!vidEnd) {
        startP = false; //Wait for image to be process
        m.lock(); //Lock imgL from other thread access

        capr = capL.read(imgL); //Get video frame
        imgGray = imgL.clone();
        if (!capr) {
            std::cout << "Capture read error" << std::endl;
            vidEnd = true;
            break;
        }

        cvtColor(imgGray, imgGray, COLOR_YUV2GRAY_I420); //Convert to grayscale for detector
        cvtColor(imgL, imgL, COLOR_YUV2BGR_I420); //Convert to BGR to display later

        pts = fDetector(imgGray); //Feature detector (FAST)
        imgL = processImg(imgL, pts, fps); //Draw the feature point(s)on the img/vid

        std::cout << "MDT ";	//Detector
        // showIMG(displayIMG(imgL,pts,fps));

        startP = true;
        m.unlock(); //Release var lock

        // FPS
        frameCounter++;
        std::time_t timeNow = std::time(0) - timeBegin;
        if (timeNow - tick >= 1) {
            tick++;
            fps = frameCounter;
            // std::cout << "Frames per second: " << frameCounter << std::endl;
            frameCounter = 0;
        }

        // Deinitialize the pyramid pool (for consecutive frames)
        PyramidPool::deinit();
        std::cout << "==="<< std::endl;
    }

    //When video has ended
    capL.release();
    destroyAllWindows();

}

// Display thread (DSP)
void showIMG()
{
    std::cout << "Starting loop..." << std::endl;
    while (1) {
        if (startP) {
            std::cout << "DSP ";
            namedWindow("Feature detection", WINDOW_NORMAL);
            cv::imshow("Feature detection", imgL);

            // ESC to escape
            int keycode = waitKey(30) & 0xff;
            if (keycode == 27){
                break;
}
        }
    }
vidEnd=true;
}

int main(int argc, char** argv)
{
    //Read video file (Finds for "a.mp4" in current directory)
    VideoCapture capL("filesrc location=a.mp4 ! qtdemux name=demux.video_0 ! queue ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw, format=(string)I420 ! appsink", CAP_GSTREAMER);

    if (!capL.isOpened()) {
        std::cout << "Failed to open camera." << std::endl;
        return (-1);
    }

    //Start Threads
    auto t1 = std::async(runProcess, capL); //Main feature detector thread
    std::thread t2(showIMG); //Display thread

    t2.join(); //Join to main thread


    return 0;
}
