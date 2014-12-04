cmake_minimum_required(VERSION 2.8)
project( uFaceDetect )
find_package( OpenCV REQUIRED )
add_executable( uFaceDetect ufacedetect.cpp )
target_link_libraries( uFaceDetect ${OpenCV_LIBS} )

