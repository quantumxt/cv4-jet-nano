#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>

using namespace cv;

int main(int argc, char** argv){
	std::cout << "Reading IMG..." << std::endl;
	
	Mat testImg = imread(argv[1], IMREAD_COLOR); 		//Load img from cmd line
	//Mat testImg = imread("t.jpg",IMREAD_COLOR);
	Mat grayImg = imread(argv[1],IMREAD_GRAYSCALE);	//Grayscale
	//Mat testImg = imread("t.jpg",IMREAD_UNCHANGED);	//Transparent PNG/TIFF
	//Mat testImg = imread("t.jpg, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);	//Read 16-bit/channel color Image, example camera RAW images

	if( testImg.empty()){
		std::cout << "IMG is empty!" << std::endl;
		return -1;
	}

	namedWindow("Color", WINDOW_NORMAL);
	namedWindow("Gray", WINDOW_AUTOSIZE);				
	//namedWindow("Color", WINDOW_OPENGL);		//With openCV support
	//namedWindow("Color", WINDOW_AUTOSIZE);	//Cannot resize window
	imshow("Color",testImg);
	imshow("Gray",grayImg);
	waitKey(0);
	destroyWindow("Color");
	return 0;
}
