cmake_minimum_required(VERSION 2.8)
project( camcv_vid1 )
find_package( OpenCV REQUIRED )
add_executable( camcv_vid1 camcv_vid1.cpp )
target_link_libraries( camcv_vid1 ${OpenCV_LIBS} )

