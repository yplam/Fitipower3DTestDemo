cmake_minimum_required(VERSION 2.8)
project( VideoCapture )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} /usr/include )
LINK_DIRECTORIES(/usr/lib)
add_executable( VideoCapture VideoCapture.cpp )
target_link_libraries( VideoCapture ${OpenCV_LIBS} webcam)

