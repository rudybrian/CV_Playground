#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int, char**) {
    cv::VideoCapture vcap;
    cv::Mat image;

    // This works on a D-Link CDS-932L
    //const std::string videoStreamAddress = "http://admin:admin@192.168.69.52/cgi/mjpg/mjpg.cgi";
    const std::string videoStreamAddress = "http://admin:admin@192.168.69.52/cgi/mjpg/mjpg.cgi?bogus=foo.mjpg";
    //const std::string videoStreamAddress = "http://admin:admin@192.168.69.52/cgi/jpg/image.cgi?foo";
    //const std::string videoStreamAddress = "rtsp://admin:admin@192.168.69.52:554/mpeg4";
    //const std::string videoStreamAddress = "rtsp://admin:admin@192.168.69.52:554/3gp";
   
    //open the video stream and make sure it's opened
    if(!vcap.open(videoStreamAddress)) {
    	std::cout << "Error opening video stream or file" << std::endl;
    	return -1;
    }
    
    for(;;) {
    	if(!vcap.read(image)) {
    		std::cout << "No frame" << std::endl;
    		cv::waitKey();
    	}
    	cv::imshow("Output Window", image);
    	if(cv::waitKey(1) >= 0) break;
    }
}

