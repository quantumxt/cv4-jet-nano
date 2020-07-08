/*
 * omni_rectify.cpp
 * Omnidirectional Image Rectification
 *
 *  __ _  _   ___ ______ ____                    _                   
 * /_ | || | / _ \____  / __ \                  | |                  
 *  | | || || (_) |  / / |  | |_   _  __ _ _ __ | |_ _   _ _ __ ___  
 *  | |__   _> _ <  / /| |  | | | | |/ _` | '_ \| __| | | | '_ ` _ \ 
 *  | |  | || (_) |/ / | |__| | |_| | (_| | | | | |_| |_| | | | | | |
 *  |_|  |_| \___//_/   \___\_\\__,_|\__,_|_| |_|\__|\__,_|_| |_| |_|
 *
 * Copyright (C) 2020 1487Quantum
 * 
 * 
 * Licensed under the MIT License.
 * 
 */

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/ccalib/omnidir.hpp"
#include <iostream>

static cv::Mat node2array(const cv::FileNode& param, const std::string& sheader, const int& aWidth, const int& aHeight)
{
    int x{ 0 }, y{ 0 };
    double tMat_array[aHeight][aWidth] = {};

    std::cout << "\n" << sheader << ":" << std::endl;
    cv::FileNodeIterator it = param.begin(), it_end = param.end();
    for (; it != it_end; ++it) {
        std::cout << (double)*it << "\t";
        tMat_array[y][x - (3 * y)] = (double)*it; //x have to be offset
        if (((x + 1) % aHeight == 0) && aHeight > 1) {
            std::cout << std::endl;
            ++y;
        }
        ++x;
    }
    std::cout << std::endl;
    cv::Mat tMat(aWidth, aHeight, CV_64F);
    std::memcpy(tMat.data, tMat_array, aHeight * aWidth * sizeof(double));
    //std::cout << tMat << std::endl;
    return tMat;
}

int err(const std::string& msg, const int& rval)
{
    std::cerr << msg << std::endl;
    return rval;
}

int main(int argc, char** argv)
{
    if (argc < 4) // Check the number of parameters
        return err((std::string) "\nUsage: " + argv[0] + "  [CALIBRATION_FILE]  [IMG_TO_DISTORT]  [ZOOM_OUT_LEVEL]\n", 1);

    const float zoomOut = atof(argv[3]); //Best around 2-6, negative would flip image horizontal + vertical
    if (zoomOut < 1.0 || zoomOut > 7.0)
        return err((std::string) "\nZOOM_OUT_LEVEL invalid:" + std::to_string(zoomOut) + "\nPlease enter range between 1.0 <-> 7.0\n", 1);

    std::string filename{ argv[1] }; //1st arg
    std::cout << "Reading calibration file: " << filename << "\nTarget IMG: " << argv[2] << "\nZOOM_OUT_LEVEL: " << zoomOut << std::endl;
    cv::FileStorage fs(filename, cv::FileStorage::READ);

    if (!fs.isOpened())
        return err("Error reading calibration file...", -1);

    cv::FileNode n = fs.getFirstTopLevelNode();
    if (n.name() != "calibration_time")
        return err("Invalid calibration file!", -1);

    //Get Calibrated Camera matrix (K)
    cv::FileNode mData = fs["camera_matrix"]["data"];
    cv::Mat kMat = node2array(mData, "Camera Matrix (K)", 3, 3);

    //Get distortion coeff (D): Distortion parameters (k1,k2,p1,p2)
    cv::FileNode dcf = fs["distortion_coefficients"]["data"];
    cv::Mat dMat = node2array(dcf, "Distortion Coeff (D)", 4, 1);

    //Get Xi
    cv::FileNode node_xi = fs["xi"];
    cv::Mat xiMat = node2array(node_xi, "Xi", 1, 1);

    cv::Mat distorted = cv::imread(argv[2], cv::IMREAD_COLOR); //2nd arg, target image
    if (distorted.empty())
        return err("Could not read img...", -1);

    cv::Mat undistorted;
    cv::Size new_size(distorted.cols, distorted.rows);

    const int centerX{ new_size.width / 2 };
    const int centerY{ new_size.height / 2 };

    constexpr int flags_out = cv::omnidir::RECTIFY_PERSPECTIVE;
    constexpr float aspectRatio{ 1.7 };

    cv::Matx33f Knew = cv::Matx33f(new_size.width / (aspectRatio * zoomOut), 0, centerX,
        0, new_size.height / zoomOut, centerY,
        0, 0, 1);

    std::cout << "\nRectifying IMG..." << std::endl;
    cv::omnidir::undistortImage(distorted, undistorted, kMat, dMat, xiMat, flags_out, Knew, new_size);

    kMat.release();
    dMat.release();
    xiMat.release();

    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Undistort", cv::WINDOW_NORMAL);

    cv::imshow("Original", distorted);
    cv::imshow("Undistort", undistorted);
    cv::waitKey(0);

    cv::destroyWindow("Original");
    cv::destroyWindow("Undistort");

    return 0;
}
