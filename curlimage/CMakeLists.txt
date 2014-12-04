cmake_minimum_required(VERSION 2.8)
project( curlimage )
find_package( OpenCV REQUIRED )
add_executable( curlimage curlimage.cpp )
target_link_libraries( curlimage ${OpenCV_LIBS} curl )

