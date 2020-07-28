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
#include <opencv2/imgproc.hpp> // drawing shapes
#include <opencv2/ximgproc.hpp>
#include "opencv2/ccalib/omnidir.hpp"
   #include "opencv2/xfeatures2d.hpp"
#include <opencv2/features2d.hpp>
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

void draw_epipolar(const cv::Mat& imgSrc, const int& numLines)
{
    for (int i = 1; i < imgSrc.rows; i += imgSrc.rows / numLines) {
        int thicknessLine = 2;
        cv::Scalar colorLine(0, 0, 255); // Green
        cv::Point p1(0, i);
        cv::Point p2(imgSrc.cols, i);
        cv::line(imgSrc, p1, p2, colorLine, thicknessLine);
    }
/*
 cv::Mat imgchannel[6], locMat, clrMat;
    cv::split(pointCloud, imgchannel);

    std::vector<cv::Mat> channelsTmp;
    channelsTmp.push_back(imgchannel[0]);
    channelsTmp.push_back(imgchannel[1]);
    channelsTmp.push_back(imgchannel[2]);
    cv::merge(channelsTmp, locMat);

    std::vector<cv::Mat> clrTmp;
    clrTmp.push_back(imgchannel[3]);
    clrTmp.push_back(imgchannel[4]);
    clrTmp.push_back(imgchannel[5]);
    cv::merge(clrTmp, clrMat);
*/
}

void splitChannels(const cv::Mat &imgSrc, cv::Mat &imgOut, const int &tsize, const int &start, const int &stop){
 cv::Mat imgchannel[tsize];
    cv::split(imgSrc, imgchannel);
 std::vector<cv::Mat> channelsTmp;
for(int k=start;k<stop+1;++k)
    channelsTmp.push_back(imgchannel[k]);
    cv::merge(channelsTmp, imgOut);
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

    //   constexpr int flags_out = cv::omnidir::RECTIFY_PERSPECTIVE, RECTIFY_LONGLATI;
    constexpr int flags_out = cv::omnidir::RECTIFY_PERSPECTIVE;
    constexpr float aspectRatio{ 1.7 };

    /*
    cv::Matx33f Knew = cv::Matx33f(new_size.width / (aspectRatio * zoomOut), 0, centerX,
        0, new_size.height / zoomOut, centerY,
        0, 0, 1);
*/
    cv::Matx33f Knew = cv::Matx33f(new_size.width / 3.142, 0, flags_out == cv::omnidir::RECTIFY_PERSPECTIVE ? centerX : 0,
        0, new_size.height / 3.142, flags_out == cv::omnidir::RECTIFY_PERSPECTIVE ? centerY : 0,
        0, 0, 1);

    std::cout << "\nRectifying IMG..." << std::endl;

    cv::Size imgSize = distorted_l.size();
    int numDisparities = 16 * 15;
    int SADWindowSize = 9;
    cv::Mat disMap;
    int pointType = cv::omnidir::XYZRGB; //cv::omnidir::XYZrgb
    // the range of theta is (0, pi) and the range of phi is (0, pi)

    cv::Mat imageRec1, imageRec2, pointCloud;
    cv::omnidir::stereoReconstruct(distorted_l, distorted_r, kMat_l, dMat_l, xiMat_l, kMat_r, dMat_r, xiMat_r, rMat, tMat, flags_out, numDisparities, SADWindowSize, disMap, imageRec1, imageRec2, imgSize, Knew, pointCloud);


//Sift
  cv::Ptr<cv::Feature2D> f2d = cv::xfeatures2d::SIFT::create();

  std::vector<cv::KeyPoint> keypoints_1, keypoints_2;    
  f2d->detect( imageRec1, keypoints_1 );
  f2d->detect( imageRec2, keypoints_2 );

cv::Mat output1,output2;
    cv::drawKeypoints(imageRec1, keypoints_1, output1);
    cv::drawKeypoints(imageRec2, keypoints_2, output2);

 cv::namedWindow("uu", cv::WINDOW_NORMAL);
    hconcat(output1, output2, output1);
    cv::imshow("uu", output1);
    cv::waitKey(0);

 cv::Mat descriptors_1, descriptors_2;    
  f2d->compute( imageRec1, keypoints_1, descriptors_1 );
  f2d->compute( imageRec2, keypoints_2, descriptors_2 );

//Matcher
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<cv::DMatch> > knn_matches;
    matcher->knnMatch( descriptors_1, descriptors_2, knn_matches, 2 );
    //-- Filter matches using the Lowe's ratio test
    const float ratio_thresh = 0.7f;
    std::vector<cv::DMatch> good_matches;

std::vector<cv::Point2f> pts1;
std::vector<cv::Point2f> pts2;

    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
	pts1.push_back(keypoints_1[i].pt);
	pts2.push_back(keypoints_2[i].pt);
        }
    }

/*
 cv::BFMatcher matcher;
  std::vector< cv::DMatch > matches;
  matcher.match( descriptors_1, descriptors_2, matches );
*/

    //-- Draw matches
cv::Mat outout;
cv::drawMatches(imageRec1,keypoints_1,imageRec2,keypoints_2,good_matches,outout, cv::Scalar::all(-1),cv::Scalar::all(-1), std::vector< char >(),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

cv::namedWindow("lll", cv::WINDOW_NORMAL);
    cv::imshow("lll", outout);
    cv::waitKey(0);

cv::Mat maskF;
//Find fundamental matrix
cv::Mat fundamentalMatrix = cv::findFundamentalMat(pts1,pts2,cv::FM_RANSAC,3,0.8, maskF);
std::cout<<fundamentalMatrix<<std::endl;

std::cout<<pts1<<std::endl;
//Compute epilines
    std::vector<cv::Vec3d> leftLines, rightLines;
    cv::computeCorrespondEpilines(pts1, 1, fundamentalMatrix, rightLines);
    cv::computeCorrespondEpilines(pts2, 2, fundamentalMatrix, leftLines);

    cv::Mat imagePointLeftCameraMatrix=cv::Mat_<double>(3,1);

for(std::size_t i=0;i<rightLines.size();i=i+1)
    {
        cv::Vec3d l=rightLines.at(i);
	double a=l.val[0];
        double b=l.val[1];
        double c=l.val[2];
        std::cout<<"------------------------a,b,c Using OpenCV (ax+by+c=0)------------------------------"<<std::endl;
        std::cout<< a <<", "<<b <<", "<<c <<std::endl;
        std::cout<<"------------------------calculating a,b,c (ax+by+c=0) ------------------------------"<<std::endl;
        
        imagePointLeftCameraMatrix.at<double>(0,0)=pts1[i].x;
        imagePointLeftCameraMatrix.at<double>(1,0)=pts1[i].y;
        imagePointLeftCameraMatrix.at<double>(2,0)=1;
        cv::Mat rightLineMatrix=fundamentalMatrix*imagePointLeftCameraMatrix;
        
        std::cout<< rightLineMatrix.at<double>(0,0) <<", "<<rightLineMatrix.at<double>(0,1) <<", "<<rightLineMatrix.at<double>(0,2) <<std::endl;

        /////////////////////////////////drawing the line on the image/////////////////////////////////
        /*ax+by+c=0*/
        double x0,y0,x1,y1;
        x0=0;
        y0=(-c-a*x0)/b;
	x1=imageRec2.cols;
        y1=(-c-a*x1)/b;

	std::cout<<"error: "<< a*pts2.at(i).x+ b*pts2.at(i).y +c<<std::endl;
	cv::line(imageRec2, cv::Point(x0,y0), cv::Point(x1,y1), cvScalar(0,255,0), 1);
    }
cv::namedWindow("kkk", cv::WINDOW_NORMAL);
    cv::imshow("kkk", imageRec1);
    cv::waitKey(0);

/*
//keep only the "good" points, remove the rest
    std::vector<cv::Point>::iterator it1, it2; 
    it1 = pts1.begin(); 
    it2 = pts2.begin(); 
for(int i=0;i<maskF.rows;++i) {
if(maskF.at<cv::Vec3b>(cv::Point(0,i))!=(cv::Vec3b)1){		//Keep 1, remove 0
pts1.erase(it1);
pts2.erase(it2);
//std::cout << i << std::endl;
}
++it1;
++it2;
}
*/


    //// viz
    cv::viz::Viz3d window("Coordinate Frame");
    window.showWidget("Coordinate Widget", cv::viz::WCoordinateSystem());

    //std::cout<<pointCloud <<std::endl;

    cv::viz::Viz3d viewer;

    //Split into 2 matrix
    cv::Mat locMat, clrMat;
    splitChannels(pointCloud,locMat,6, 0,2);
    splitChannels(pointCloud,clrMat, 6, 3,5);

    clrMat.convertTo(clrMat, CV_8U); //Convert from CV_32F C3 to CV_8U C3	//https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv#17820615

    viewer = cv::viz::Viz3d("Point Cloud");
    cv::viz::WCloud cloud_widget = cv::viz::WCloud(locMat, clrMat);
    viewer.showWidget("Cloud", cloud_widget);
    //    viewer.spinOnce(1,true);

    //Rotate view
    cv::Mat rot_vec = cv::Mat::zeros(1, 3, CV_32F);
    float translation_phase = 0.0, translation = 0.0;
    while (!viewer.wasStopped()) {
        /* Rotation using rodrigues */
        /// Rotate around (1,1,1)
        rot_vec.at<float>(0, 0) += 3.142 * 0.01f;
        rot_vec.at<float>(0, 1) += 3.142 * 0.01f;
        rot_vec.at<float>(0, 2) += 3.142 * 0.01f;

        /// Shift on (1,1,1)
        translation_phase += 3.142 * 0.01f;
        translation = sin(translation_phase);

        cv::Rodrigues(rot_vec, locMat);

        /// Construct pose
        cv::Affine3f pose(locMat, cv::Vec3f(translation, translation, translation));

        viewer.setWidgetPose("Cloud", pose);

        viewer.spinOnce(1, true);
    }

    //sgbm
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, numDisparities, SADWindowSize);

    // param
    int sgbmWinSize = 9; //5	Range: 3 - 11, odd num
    int numberOfDisparities = 16 * 15;
    int cn = 3;

    // filter
    cv::Ptr<cv::ximgproc::DisparityWLSFilter> wls_filter;
    wls_filter = cv::ximgproc::createDisparityWLSFilter(sgbm);
    cv::Ptr<cv::StereoMatcher> sm = cv::ximgproc::createRightMatcher(sgbm);

    // init
        sgbm->setMinDisparity(-10);
    sgbm->setPreFilterCap(30); //30,100
    sgbm->setBlockSize(sgbmWinSize);
    sgbm->setP1(8 * cn * sgbmWinSize * sgbmWinSize);
    sgbm->setP2(32 * cn * sgbmWinSize * sgbmWinSize);
    sgbm->setMode(cv::StereoSGBM::MODE_SGBM); //MODE_SGBM_3WAY, MODE_HH (8Ways)

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

    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::namedWindow("Undistort", cv::WINDOW_NORMAL);
    cv::namedWindow("pcl", cv::WINDOW_NORMAL);

    //Original
    hconcat(distorted_l, distorted_r, distorted_l);
    draw_epipolar(distorted_l, 15);
    cv::imshow("Original", distorted_l);
    //Undistort
    hconcat(imageRec1, imageRec2, imageRec1);
    draw_epipolar(imageRec1, 15);
    cv::imshow("Undistort", imageRec1);

    cv::imshow("pcl", disMap);
    cv::waitKey(0);

    cv::destroyWindow("Original");
    cv::destroyWindow("Undistort");
    cv::destroyWindow("pcl");
    cv::destroyWindow("disparity");
    return 0;
}
