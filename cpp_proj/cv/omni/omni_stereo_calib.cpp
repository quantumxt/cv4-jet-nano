/*
 * omni_stereo_calib.cpp
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

static bool detectChessboardCorners(const std::vector<std::string>& list_l, std::vector<std::string>& list_detected_l,
    std::vector<cv::Mat>& imagePoints_l, const std::vector<std::string>& list_r, std::vector<std::string>& list_detected_r,
    std::vector<cv::Mat>& imagePoints_r, const cv::Size& boardSize, cv::Size& imageSize)
{
    imagePoints_l.resize(0);
    list_detected_l.resize(0);
    imagePoints_r.resize(0);
    list_detected_r.resize(0);
    int n_img = (int)list_l.size();
    cv::Mat img_l, img_r;
    std::cout << "Finding corners:" << std::endl;
    for (int i{ 0 }; i < n_img; ++i) {
        cv::Mat points_l, points_r;
        img_l = cv::imread(list_l[i], cv::IMREAD_GRAYSCALE);
        img_r = cv::imread(list_r[i], cv::IMREAD_GRAYSCALE);
        if (img_l.empty() || img_r.empty()) {
            std::cout << "[ \x1B[1m?\033[0m ] " << list_l[i] << std::endl; //Can't find image [1m: Bold
            continue;
        }
        bool found_l{ findChessboardCorners(img_l, boardSize, points_l) };
        bool found_r{ findChessboardCorners(img_r, boardSize, points_r) };
        if (found_l && found_r) {
            std::cout << "[ \x1B[32m✔\033[0m ] "; //Found corners, 32: green, colors: https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c
            if (points_l.type() != CV_64FC2)
                points_l.convertTo(points_l, CV_64FC2);
            if (points_r.type() != CV_64FC2)
                points_r.convertTo(points_r, CV_64FC2);
            imagePoints_l.push_back(points_l);
            list_detected_l.push_back(list_l[i]);
            imagePoints_r.push_back(points_r);
            list_detected_r.push_back(list_r[i]);
        }
        else
            std::cout << "[ \x1B[31m✘\033[0m ] "; //No corners or either have missing corners
        std::cout << list_l[i] << std::endl;
    }
    std::cout << "\nImages used: " << list_detected_l.size() << std::endl;
    if (!img_l.empty() && !img_r.empty())
        imageSize = img_l.size();

    if (imagePoints_l.size() < 3 || imagePoints_r.size() < 3)
        return false;
    else
        return true;
}

static void saveCameraParams(const std::string& filename, const int flags, const cv::Mat& cameraMatrix1, const cv::Mat& cameraMatrix2, const cv::Mat& distCoeffs1,
    const cv::Mat& disCoeffs2, const double xi1, const double xi2, const cv::Vec3d rvec, const cv::Vec3d tvec,
    const std::vector<cv::Vec3d>& rvecs, const std::vector<cv::Vec3d>& tvecs, std::vector<std::string> detec_list_1, std::vector<std::string> detec_list_2,
    const cv::Mat& idx, const double rms, const std::vector<cv::Mat>& imagePoints1, const std::vector<cv::Mat>& imagePoints2)
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

    fs << "camera_matrix_1" << cameraMatrix1;
    fs << "distortion_coefficients_1" << distCoeffs1;
    fs << "xi_1" << xi1;

    fs << "camera_matrix_2" << cameraMatrix2;
    fs << "distortion_coefficients_2" << disCoeffs2;
    fs << "xi_2" << xi2;

    fs << "rvec" << rvec;
    fs << "tvec" << tvec;

    cv::Mat om_t(1, 6, CV_64F);
    cv::Mat(rvec).reshape(1, 1).copyTo(om_t.colRange(0, 3));
    cv::Mat(tvec).reshape(1, 1).copyTo(om_t.colRange(3, 6));
    //cvWriteComment( *fs, "6-tuples (rotation vector + translation vector) for each view", 0 );
    fs << "extrinsic_parameters" << om_t;

    if (!rvecs.empty() && !tvecs.empty()) {
        cv::Mat rvec_tvec((int)rvecs.size(), 6, CV_64F);
        for (int i = 0; i < (int)rvecs.size(); ++i) {
            cv::Mat(rvecs[i]).reshape(1, 1).copyTo(rvec_tvec(cv::Rect(0, i, 3, 1)));
            cv::Mat(tvecs[i]).reshape(1, 1).copyTo(rvec_tvec(cv::Rect(3, i, 3, 1)));
        }
        //cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "extrinsic_parameters_1" << rvec_tvec;
    }

    fs << "rms" << rms;

    //cvWriteComment( *fs, "names of images that are acturally used in calibration", 0 );
    fs << "used_imgs_1"
       << "[";
    for (int i = 0; i < (int)idx.total(); ++i) {
        fs << detec_list_1[(int)idx.at<int>(i)];
    }
    fs << "]";

    fs << "used_imgs_2"
       << "[";
    for (int i = 0; i < (int)idx.total(); ++i) {
        fs << detec_list_2[(int)idx.at<int>(i)];
    }
    fs << "]";

    if (!imagePoints1.empty()) {
        cv::Mat imageMat((int)imagePoints1.size(), (int)imagePoints1[0].total(), CV_64FC2);
        for (int i = 0; i < (int)imagePoints1.size(); ++i) {
            cv::Mat r = imageMat.row(i).reshape(2, imageMat.cols);
            cv::Mat imagei(imagePoints1[i]);
            imagei.copyTo(r);
        }
        fs << "image_points_1" << imageMat;
    }

    if (!imagePoints2.empty()) {
        cv::Mat imageMat((int)imagePoints2.size(), (int)imagePoints2[0].total(), CV_64FC2);
        for (int i = 0; i < (int)imagePoints2.size(); ++i) {
            cv::Mat r = imageMat.row(i).reshape(2, imageMat.cols);
            cv::Mat imagei(imagePoints2[i]);
            imagei.copyTo(r);
        }
        fs << "image_points_2" << imageMat;
    }

    std::cout << "\n=== Calibration Done! ===\n" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 6)
        return err((std::string) "\nUsage: " + argv[0] + "  [IMG_LIST_LEFT]  [IMG_LIST_RIGHT] [CHECKBOARD_HORIZONTAL_POINTS]   [CHECKBOARD_VERTICAL_POINTS]  [SQUARE_WIDTH (mm)]\n", 1);

    if (atoi(argv[3]) <= 2 || atoi(argv[4]) <= 2)
        return err("\n[CHECKBOARD_HORIZONTAL_POINTS] & [CHECKBOARD_VERTICAL_POINTS] have to be > 2!\n", 2);

    if (atof(argv[5]) <= 0.00)
        return err("\n[SQUARE_WIDTH] have to be > 0.0!\n", 2);

    cv::Size boardSize(atoi(argv[3]), atoi(argv[4])); //8 x 5
    cv::Size imageSize; //Img width & height

    constexpr int flags = cv::omnidir::CALIB_FIX_SKEW + cv::omnidir::CALIB_FIX_CENTER;
    const char* outputFilename = "./out_camera_params_stereo.xml"; //Save in current working directory

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

    std::vector<cv::Mat> objectPoints, imagePoints_L, imagePoints_R;
    std::vector<std::string> image_list_L, detect_list_L, image_list_R, detect_list_R; // get image name list

    if (!readStringList(argv[1], image_list_L) || !readStringList(argv[2], image_list_R))
        return err("Failed to read image list!\n", -1);

    // find corners in images
    // some images may fail automatic corner detection, images detected are in detec_list
    if (!detectChessboardCorners(image_list_L, detect_list_L, imagePoints_L, image_list_R, detect_list_R, imagePoints_R, boardSize, imageSize))
        return err("Not enough corner detected images!\n", -1);

    // calculate object coordinates
    cv::Mat object;
    calcChessboardCorners(boardSize, square_width, object);
    for (int i{ 0 }; i < (int)detect_list_L.size(); ++i)
        objectPoints.push_back(object);

    // run calibration, some images are discarded in calibration process because they are failed
    // in initialization. Retained image indexes are in idx variable.
    cv::Mat K1, K2, D1, D2, xi1, xi2, idx;
    std::vector<cv::Vec3d> rvecs, tvecs;
    cv::Vec3d rvec, tvec;
    double _xi1, _xi2, rms;

    cv::TermCriteria criteria(3, 200, 1e-8);

    //  Mat newK;
    //  fisheye::estimateNewCameraMatrixForUndistortRectify(K, D, imageSize, Matx33d::eye(), newK, 1);

    rms = cv::omnidir::stereoCalibrate(objectPoints, imagePoints_L, imagePoints_R, imageSize, imageSize, K1, xi1, D1, K2, xi2, D2, rvec, tvec, rvecs, tvecs, flags, criteria, idx);

    _xi1 = xi1.at<double>(0);
    _xi2 = xi2.at<double>(0);

    saveCameraParams(outputFilename, flags, K1, K2, D1, D2, _xi1, _xi2, rvec, tvec, rvecs, tvecs, detect_list_L, detect_list_R, idx, rms, imagePoints_L, imagePoints_R);

    return 0;
}
