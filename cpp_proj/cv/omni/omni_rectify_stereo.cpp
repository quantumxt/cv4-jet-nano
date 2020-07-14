/*
 * omni_rectify_stereo.cpp
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
#include "opencv2/viz.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/calib3d.hpp>
#include <opencv2/ximgproc.hpp>
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
    if (argc < 5) // Check the number of parameters
        return err((std::string) "\nUsage: " + argv[0] + "  [CALIBRATION_FILE]  [IMG_TO_DISTORT_LEFT]  [IMG_TO_DISTORT_RIGHT]  [ZOOM_OUT_LEVEL]\n", 1);

    const float zoomOut = atof(argv[4]); //Best around 2-6, negative would flip image horizontal + vertical
    if (zoomOut < 1.0 || zoomOut > 7.0)
        return err((std::string) "\nZOOM_OUT_LEVEL invalid:" + std::to_string(zoomOut) + "\nPlease enter range between 1.0 <-> 7.0\n", 1);

    std::string filename{ argv[1] }; //1st arg
    std::cout << "Reading calibration file: " << filename << "\nTarget Left: " << argv[2] << "\nTarget Right: " << argv[3] << "\nZOOM_OUT_LEVEL: " << zoomOut << std::endl;
    cv::FileStorage fs(filename, cv::FileStorage::READ);

    if (!fs.isOpened())
        return err("Error reading calibration file...", -1);

    cv::FileNode n = fs.getFirstTopLevelNode();
    if (n.name() != "calibration_time")
        return err("Invalid calibration file!", -1);

    //Get Calibrated Camera matrix (K)
    cv::FileNode mData1 = fs["camera_matrix_1"];
    cv::Mat kMat_l = node2array(mData1["data"], "Camera Matrix (K) Left", mData1["rows"], mData1["cols"]);

    //Get distortion coeff (D): Distortion parameters (k1,k2,p1,p2)
    cv::FileNode dcf1 = fs["distortion_coefficients_1"];
    cv::Mat dMat_l = node2array(dcf1["data"], "Distortion Coeff (D) Left", dcf1["rows"], dcf1["cols"]);

    //Get Xi
    cv::FileNode node_xi1 = fs["xi_1"];
    cv::Mat xiMat_l = node2array(node_xi1, "Xi_l", 1, 1);

    //Get Calibrated Camera matrix (K)
    cv::FileNode mData2 = fs["camera_matrix_2"];
    cv::Mat kMat_r = node2array(mData2["data"], "Camera Matrix (K) Right", mData2["rows"], mData2["cols"]);

    //Get distortion coeff (D): Distortion parameters (k1,k2,p1,p2)
    cv::FileNode dcf2 = fs["distortion_coefficients_2"];
    cv::Mat dMat_r = node2array(dcf2["data"], "Distortion Coeff (D) Right", dcf2["rows"], dcf2["cols"]);

    //Get Xi
    cv::FileNode node_xi2 = fs["xi_2"];
    cv::Mat xiMat_r = node2array(node_xi2, "Xi_r", 1, 1);

    //Rotation & translational vector
    cv::FileNode fnr = fs["rvec"];
    cv::Mat rMat = node2array(fnr, "Rotation (R)", 3, 1);

    cv::FileNode fnt = fs["tvec"];
    cv::Mat tMat = node2array(fnt, "Translation (T)", 3, 1);
    std::cout << "\n===\n";

    cv::Mat distorted_l = cv::imread(argv[2], cv::IMREAD_COLOR); //2nd arg, target image
    cv::Mat distorted_r = cv::imread(argv[3], cv::IMREAD_COLOR); //2nd arg, target image
    if (distorted_l.empty())
        return err("Could not read img...", -1);

    cv::Size new_size(distorted_l.cols, distorted_l.rows);

    const int centerX{ new_size.width / 2 };
    const int centerY{ new_size.height / 2 };

    //   constexpr int flags_out = cv::omnidir::RECTIFY_PERSPECTIVE;
    constexpr int flags_out = cv::omnidir::RECTIFY_LONGLATI;
    constexpr float aspectRatio{ 1.7 };

    /*
    cv::Matx33f Knew = cv::Matx33f(new_size.width / (aspectRatio * zoomOut), 0, centerX,
        0, new_size.height / zoomOut, centerY,
        0, 0, 1);
*/
    cv::Matx33f Knew = cv::Matx33f(new_size.width / 3.142, 0, 0,
        0, new_size.height / 3.142, 0,
        0, 0, 1);

    std::cout << "\nRectifying IMG..." << std::endl;

    cv::Size imgSize = distorted_l.size();
    int numDisparities = 16 * 20;
    int SADWindowSize = 20;
    cv::Mat disMap;
    int pointType = cv::omnidir::XYZ; //cv::omnidir::XYZrgb
    // the range of theta is (0, pi) and the range of phi is (0, pi)

    cv::Mat imageRec1, imageRec2, pointCloud;
    cv::omnidir::stereoReconstruct(distorted_l, distorted_r, kMat_l, dMat_l, xiMat_l, kMat_r, dMat_r, xiMat_r, rMat, tMat, flags_out, numDisparities, SADWindowSize, disMap, imageRec1, imageRec2, imgSize, Knew, pointCloud);


    //sgbm
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, numDisparities, SADWindowSize);

    // param
    int sgbmWinSize = 17;
    int numberOfDisparities = 16 * 10;
    int cn = 3;

    // filter
    cv::Ptr<cv::ximgproc::DisparityWLSFilter> wls_filter;
    wls_filter = cv::ximgproc::createDisparityWLSFilter(sgbm);
    cv::Ptr<cv::StereoMatcher> sm = cv::ximgproc::createRightMatcher(sgbm);

    // init
    sgbm->setPreFilterCap(100);
    sgbm->setBlockSize(sgbmWinSize);
    sgbm->setP1(8 * cn * sgbmWinSize * sgbmWinSize);
    sgbm->setP2(32 * cn * sgbmWinSize * sgbmWinSize);
    sgbm->setMode(cv::StereoSGBM::MODE_SGBM_3WAY);

    //// viz
    cv::viz::Viz3d window("Coordinate Frame");
    window.showWidget("Coordinate Widget", cv::viz::WCoordinateSystem());

    cv::Mat disparity16S, img16Sr;

    sgbm->compute(imageRec1, imageRec2, disparity16S);
    sm->compute(imageRec2, imageRec1, img16Sr);

    cv::Mat showDisparity;
    disparity16S.convertTo(showDisparity, CV_8UC1, 255 / (numberOfDisparities * 16.));

    cv::imshow("disparity", showDisparity);

    // cv::omnidir::undistortImage(distorted, undistorted, kMat, dMat, xiMat, flags_out, Knew, new_size);

    kMat_l.release();
    dMat_l.release();
    xiMat_l.release();
    kMat_r.release();
    dMat_r.release();
    xiMat_r.release();
    rMat.release();
    tMat.release();

    /*
cv::viz::Viz3d viewer;
viewer = cv::viz::Viz3d( "Point Cloud" );
    cv::viz::WCloud cloud_widget = cv::viz::WCloud( pointCloud, cv::viz::Color::green() );
viewer.showWidget( "Cloud", cloud_widget );
*/

    cv::namedWindow("Original L", cv::WINDOW_NORMAL);
    cv::namedWindow("Undistort L", cv::WINDOW_NORMAL);
    cv::namedWindow("Original R", cv::WINDOW_NORMAL);
    cv::namedWindow("Undistort R", cv::WINDOW_NORMAL);
    cv::namedWindow("pcl", cv::WINDOW_NORMAL);
    cv::imshow("Original L", distorted_l);
    cv::imshow("Undistort L", imageRec1);
    cv::imshow("Original R", distorted_r);
    cv::imshow("Undistort R", imageRec2);
    cv::imshow("pcl", disMap);
    cv::waitKey(0);

    cv::destroyWindow("Original L");
    cv::destroyWindow("Undistort L");
    cv::destroyWindow("Original R");
    cv::destroyWindow("Undistort R");
    cv::destroyWindow("pcl");
    return 0;
}
