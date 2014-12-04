cmake_minimum_required(VERSION 2.8)
project( streamtest )
find_package( OpenCV REQUIRED )
add_executable( streamtest streamtest.cpp )
target_link_libraries( streamtest ${OpenCV_LIBS} )

