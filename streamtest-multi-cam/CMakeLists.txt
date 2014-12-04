cmake_minimum_required(VERSION 2.8)
project( streamtest-multi-cam )
find_package( OpenCV REQUIRED )
add_executable( streamtest-multi-cam streamtest-multi-cam.cpp )
target_link_libraries( streamtest-multi-cam ${OpenCV_LIBS} )

