/*
 * omni_monocalib.cpp
 * Omnidirectional Image Calibration
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
#include "opencv2/imgcodecs.hpp"
#include "opencv2/ccalib/omnidir.hpp"
#include "opencv2/calib3d.hpp"
#include <iostream>
#include <vector>

static int err(const std::string& msg, const int& rval)
{
    std::cerr << msg << std::endl;
    return rval;
}

static bool readStringList(const std::string& filename, std::vector<std::string>& l)
{
    std::cout << "\nReading image list..." << std::endl;
    l.resize(0);
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened())
        return false;
    cv::FileNode n = fs.getFirstTopLevelNode();
    if (n.type() != cv::FileNode::SEQ)
        return false;
    cv::FileNodeIterator it = n.begin(), it_end = n.end();
    for (; it != it_end; ++it)
        l.push_back((std::string)*it);
    return true;
}

static void calcChessboardCorners(const cv::Size& boardSize, const double& square_width, cv::Mat& corners)
{
    std::cout << "\nCalculating corners..." << std::endl;
    // corners has type of CV_64FC3
    corners.release();
    int n{ boardSize.width * boardSize.height };
    corners.create(n, 1, CV_64FC3);
    cv::Vec3d* ptr = corners.ptr<cv::Vec3d>();
    for (int i{ 0 }; i < boardSize.height; ++i) {
        for (int j{ 0 }; j < boardSize.width; ++j) {
            ptr[i * boardSize.width + j] = cv::Vec3d(double(j * square_width), double(i * square_width), 0.0);
        }
    }
}

static bool detecChessboardCorners(const std::vector<std::string>& list, std::vector<std::string>& list_detected,
    std::vector<cv::Mat>& imagePoints, const cv::Size& boardSize, cv::Size& imageSize)
{
    imagePoints.resize(0);
    list_detected.resize(0);
    int n_img = (int)list.size();
    cv::Mat img;
    std::cout << "Finding corners..." << std::endl;
    for (int i{ 0 }; i < n_img; ++i) {
        cv::Mat points;
        img = cv::imread(list[i], cv::IMREAD_GRAYSCALE);
        if (img.empty()) {
            std::cout << "[ \x1B[1m?\033[0m ] " << list[i] << std::endl; //Can't find image [1m: Bold
            continue;
        }
        bool found{ findChessboardCorners(img, boardSize, points) };
        if (found) {
            std::cout << "[ \x1B[32m✔\033[0m ] "; //Found corners, 32: green, colors: https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
            if (points.type() != CV_64FC2)
                points.convertTo(points, CV_64FC2);
            imagePoints.push_back(points);
            list_detected.push_back(list[i]);
        }
        else
            std::cout << "[ \x1B[31m✘\033[0m ] "; //No corners
        std::cout << list[i] << std::endl;
    }
    std::cout << "\nImages used: " << list_detected.size() << std::endl;
    if (!img.empty())
        imageSize = img.size();
    if (imagePoints.size() < 3)
        return false;
    else
        return true;
}

static void saveCameraParams(const std::string& filename, int flags,
    const cv::Mat& cameraMatrix,
    const cv::Mat& distCoeffs,
    const double xi,
    const std::vector<cv::Vec3d>& rvecs,
    const std::vector<cv::Vec3d>& tvecs,
    std::vector<std::string> detec_list,
    const cv::Mat& idx,
    const double rms,
    const std::vector<cv::Mat>& imagePoints)
{
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    time_t tt;
    time(&tt);
    struct tm* t2 = localtime(&tt);
    char buf[512];
    strftime(buf, sizeof(buf) - 1, "%c", t2);

    fs << "calibration_time" << buf;

    if (!rvecs.empty())
        fs << "nFrames" << (int)rvecs.size();

    fs << "flags" << flags;

    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;
    fs << "xi" << xi;

    fs << "used_imgs" //Images used in calibration
       << "[";
    for (int i = 0; i < (int)idx.total(); ++i) {
        fs << detec_list[(int)idx.at<int>(i)];
    }
    fs << "]";

    if (!rvecs.empty() && !tvecs.empty()) {
        cv::Mat rvec_tvec((int)rvecs.size(), 6, CV_64F);
        for (int i = 0; i < (int)rvecs.size(); ++i) {
            cv::Mat(rvecs[i]).reshape(1, 1).copyTo(rvec_tvec(cv::Rect(0, i, 3, 1)));
            cv::Mat(tvecs[i]).reshape(1, 1).copyTo(rvec_tvec(cv::Rect(3, i, 3, 1)));
        }
        //a set of 6-tuples (rotation vector + translation vector) for each view
        fs << "extrinsic_parameters" << rvec_tvec;
    }

    fs << "rms" << rms;

    if (!imagePoints.empty()) {
        cv::Mat imageMat((int)imagePoints.size(), (int)imagePoints[0].total(), CV_64FC2);
        for (int i{ 0 }; i < (int)imagePoints.size(); ++i) {
            cv::Mat r = imageMat.row(i).reshape(2, imageMat.cols);
            cv::Mat imagei{ imagePoints[i] };
            imagei.copyTo(r);
        }
        fs << "image_points" << imageMat;
    }
    std::cout << "\n=== Calibration Done! ===\n" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 5)
        return err((std::string) "\nUsage: " + argv[0] + "  [IMG_LIST]  [CHECKBOARD_HORIZONTAL_POINTS]   [CHECKBOARD_VERTICAL_POINTS]  [SQUARE_WIDTH (mm)]\n", 1);

    if (atoi(argv[2]) <= 2 || atoi(argv[3]) <= 2)
        return err("\n[CHECKBOARD_HORIZONTAL_POINTS] & [CHECKBOARD_VERTICAL_POINTS] have to be > 2!\n", 2);

    if (atoi(argv[3]) <= 0.00)
        return err("\n[SQUARE_WIDTH] have to be > 0.0!\n", 2);

    cv::Size boardSize(atoi(argv[2]), atoi(argv[3])); //8 x 5
    cv::Size imageSize; //Img width & height

    constexpr int flags = cv::omnidir::CALIB_FIX_SKEW + cv::omnidir::CALIB_FIX_CENTER;
    const char* outputFilename = "./out_camera_params.xml"; //Save in current working directory

    const double square_width{ atof(argv[4]) }; //0.03;

    char buf[512]{ "None" };
    if (flags != 0) {
        sprintf(buf, "%s%s%s%s%s%s%s%s%s",
            flags & cv::omnidir::CALIB_USE_GUESS ? "use_intrinsic_guess " : "",
            flags & cv::omnidir::CALIB_FIX_SKEW ? "fix_skew " : "",
            flags & cv::omnidir::CALIB_FIX_K1 ? "fix_k1 " : "",
            flags & cv::omnidir::CALIB_FIX_K2 ? "fix_k2 " : "",
            flags & cv::omnidir::CALIB_FIX_P1 ? "fix_p1 " : "",
            flags & cv::omnidir::CALIB_FIX_P2 ? "fix_p2 " : "",
            flags & cv::omnidir::CALIB_FIX_XI ? "fix_xi " : "",
            flags & cv::omnidir::CALIB_FIX_GAMMA ? "fix_gamma " : "",
            flags & cv::omnidir::CALIB_FIX_CENTER ? "fix_center " : "");
    }

    std::cout << "\n[CONFIG]\nIMG_LIST Path:\t\t\t" << argv[1] << "\nCHECKBOARD_HORIZONTAL_POINTS:\t" << argv[2] << "\nCHECKBOARD_VERTICAL_POINTS:\t" << argv[3] << "\nSQUARE_WIDTH (mm):\t\t" << argv[4] << "\nFLAGS:\t\t\t\t" << buf << "\nOutput path:\t\t\t" << outputFilename << std::endl;

    std::vector<cv::Mat> objectPoints, imagePoints;
    std::vector<std::string> image_list, detec_list; // get image name list

    if (!readStringList(argv[1], image_list))
        return err("Failed to read image list!\n", -1);

    // find corners in images
    // some images may fail automatic corner detection, images detected are in detec_list
    if (!detecChessboardCorners(image_list, detec_list, imagePoints, boardSize, imageSize))
        return err("Not enough corner detected images!\n", -1);

    // calculate object coordinates
    cv::Mat object;
    calcChessboardCorners(boardSize, square_width, object);
    for (int i{ 0 }; i < (int)detec_list.size(); ++i)
        objectPoints.push_back(object);

    // run calibration, some images are discarded in calibration process because they are failed
    // in initialization. Retained image indexes are in idx variable.
    cv::Mat K, D, xi, idx;
    std::vector<cv::Vec3d> rvecs, tvecs;
    double _xi, rms;
    cv::TermCriteria criteria(3, 200, 1e-8);

      //  Mat newK;
      //  fisheye::estimateNewCameraMatrixForUndistortRectify(K, D, imageSize, Matx33d::eye(), newK, 1);

    rms = cv::omnidir::calibrate(objectPoints, imagePoints, imageSize, K, xi, D, rvecs, tvecs, flags, criteria, idx);
    _xi = xi.at<double>(0);
    saveCameraParams(outputFilename, flags, K, D, _xi,
        rvecs, tvecs, detec_list, idx, rms, imagePoints);

    return 0;
}
