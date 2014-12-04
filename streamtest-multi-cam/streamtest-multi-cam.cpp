//#include "opencv2/core.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/core/ocl.hpp"

#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;


int main(int, char**) {
    cv::VideoCapture vcap, vcap2;
    cv::Mat image, image2;

    // This works on a TrendNet TV-IP422WN
    //const std::string videoStreamAddress = "http://admin:admin@192.168.69.52/cgi/mjpg/mjpg.cgi";
    //const std::string videoStreamAddress = "http://admin:admin@192.168.69.52/cgi/mjpg/mjpg.cgi?bogus=foo.mjpg";
    const std::string videoStreamAddress = "http://admin:admin@192.168.69.52/cgi/jpg/image.cgi?bogus=foo.jpg";
    //const std::string videoStreamAddress = "rtsp://admin:admin@192.168.69.52:554/mpeg4";
    //const std::string videoStreamAddress = "rtsp://admin:admin@192.168.69.52:554/3gp";
   
    //open the video stream and make sure it's opened
    if(!vcap.open(videoStreamAddress)) {
    	std::cout << "Error opening video stream1 or file" << std::endl;
    	return -1;
    }
    
    if(!vcap2.open(videoStreamAddress)) {
    	std::cout << "Error opening video stream2 or file" << std::endl;
    	return -1;
    }
    
    for(;;) {
    	//if(!vcap.read(image)) {
    	//image = imread(videoStreamAddress, 1);
    	vcap >> image;
    	//	std::cout << "No frame from vcap" << std::endl;
    	//	cv::waitKey();
    	//}
    	//if(!vcap2.read(image2)) {
	//image2 = imread(videoStreamAddress, 1);
    	vcap2 >> image2;
    	//	std::cout << "No frame from vcap2" << std::endl;
    	//	cv::waitKey();
    	//}

	cv::Mat image_copy = image.clone();
	cv::Mat image2_copy = image2.clone();
	cv::Mat gray, gray2;

	// Convert the current frame to grayscale:
	cv::cvtColor(image_copy, gray, COLOR_BGR2GRAY);
	cv::cvtColor(image2_copy, gray2, COLOR_BGR2GRAY);
	// We need to equalize the histo to reduce the impact of lighting
	cv::equalizeHist(gray,gray);
	cv::equalizeHist(gray2,gray2);
	
    	cv::imshow("Output Window1", gray);
    	cv::imshow("Output Window2", gray2);
    	if(cv::waitKey(1) >= 0) break;
    }
}

