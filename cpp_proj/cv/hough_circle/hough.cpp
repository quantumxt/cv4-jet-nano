#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;

int main(int argc, char** argv)
{
    std::cout << "Reading IMG..." << std::endl;

    Mat src = imread(argv[1], IMREAD_COLOR); //Load img from cmd line
    //Mat testImg = imread("t.jpg",IMREAD_COLOR);
    Mat grayImg = imread(argv[1], IMREAD_GRAYSCALE); //Grayscale
    //Mat testImg = imread("t.jpg",IMREAD_UNCHANGED);	//Transparent PNG/TIFF
    //Mat testImg = imread("t.jpg, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);	//Read 16-bit/channel color Image, example camera RAW images

    if (src.empty()) {
        std::cout << "IMG is empty!" << std::endl;
        return -1;
    }

    medianBlur(grayImg, grayImg, 31);

    Mat dst, detected_edges;

    //Threshold
    Mat ithr;
    adaptiveThreshold(grayImg, ithr, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 191, 0);

    //Dilate
    int erosion_type = MORPH_ELLIPSE; //MORPH_RECT;
    int erosion_size{ 5 };
    Mat element = getStructuringElement(erosion_type,
        Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        Point(erosion_size, erosion_size));
    dilate(ithr, ithr, element);

    int lowThreshold = 20;
    const int max_lowThreshold = 80;
    const int ratio = 4;
    const int kernel_size = 3;
    //Canny
    Canny(ithr, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
    dst = Scalar::all(0);
    src.copyTo(dst, detected_edges);

    std::vector<Vec3f> circles;
    HoughCircles(ithr, circles, HOUGH_GRADIENT, 1,
        grayImg.rows / 20, // change this value to detect circles with different distances to each other
        100, 30, 650, 850 // change the last two parameters
        // (min_radius & max_radius) to detect larger circles
        );
    Point minPt(10000, 10000);
    int minRadius{ 10000 };
    int minDiff{ 10000 };

    //Draw center lines
    int thicknessLine{ 1 };
    cv::line(src, Point(0, grayImg.rows / 2), Point(grayImg.cols, grayImg.rows / 2), Scalar(0, 0, 255), thicknessLine);
    cv::line(src, Point(grayImg.cols / 2, 0), Point(grayImg.cols / 2, grayImg.rows), Scalar(0, 0, 255), thicknessLine);

    for (size_t i = 0; i < circles.size(); i++) {

        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);

        if (center.y > (grayImg.rows / 3) && center.y < (grayImg.rows * 2 / 3)) {

            // circle center
            circle(src, center, 1, Scalar(0, 100, 100), 3, LINE_AA);

            // circle outline
            int radius = c[2];

            int centerDiff{ abs(center.y - grayImg.rows / 2) };
            if (centerDiff < minDiff) {
                std::cout << abs(center.y - grayImg.rows / 2) << std::endl;
                std::cout << minPt.y << std::endl;
                minPt.x = center.x;
                minPt.y = center.y;
                minDiff = centerDiff;
                minRadius = radius;
            }

            circle(src, center, radius, Scalar(255, 0, 255), 3, LINE_AA);
            putText(src, (std::string) "R: " + std::to_string(radius) + ", " + std::to_string(centerDiff), Point(center.x, center.y),
                FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 200, 0), 1, CV_AA);
        }
    }
    circle(src, minPt, 1, Scalar(250, 0, 0), 3, LINE_AA);
    circle(src, minPt, minRadius, Scalar(255, 0, 0), 3, LINE_AA);

    namedWindow("Color", WINDOW_NORMAL);
    namedWindow("Gray", WINDOW_NORMAL);
    namedWindow("Canny", WINDOW_NORMAL);
    namedWindow("Threshold", WINDOW_NORMAL);
    //namedWindow("Color", WINDOW_OPENGL);		//With openCV support
    //namedWindow("Color", WINDOW_AUTOSIZE);	//Cannot resize window
    imshow("Color", src);
    imshow("Threshold", ithr);
    imshow("Canny", dst);
    imshow("Gray", grayImg);
    waitKey(0);
    destroyWindow("Color");
    return 0;
}
