cmake_minimum_required(VERSION 3.20)

project(Waveshare-Stereo-Camera)

find_package(OpenCV REQUIRED)

set(WaveShare-stereo-camera_SRC Waveshare-stereo-camera/src)
set(WaveShare-stereo-camera_INCLUDE Waveshare-stereo-camera/include)
set(WaveShare-stereo-camera_PCH Waveshare-stereo-camera/pch)

include_directories(
    ${OpenCV_INCLUDE_DIRS} 
    ${WaveShare-stereo-camera_INCLUDE}
)

set(Library_SRC
    ${WaveShare-stereo-camera_SRC}/StereoImage.cpp
    ${WaveShare-stereo-camera_SRC}/StereoCamera.cpp
)

set(Library_PCH
    ${WaveShare-stereo-camera_PCH}/ProjectHeader.pch
)

add_library(${CMAKE_PROJECT_NAME} ${Library_SRC})

target_precompile_headers(${CMAKE_PROJECT_NAME} PUBLIC ${Library_PCH})

target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS})

add_executable(${CMAKE_PROJECT_NAME}-ConnectionTest test.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME}-ConnectionTest ${CMAKE_PROJECT_NAME})