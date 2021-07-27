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
    ${WaveShare-stereo-camera_SRC}/Image.cpp
    ${WaveShare-stereo-camera_SRC}/StereoImage.cpp
    ${WaveShare-stereo-camera_SRC}/StereoCamera.cpp
    ${WaveShare-stereo-camera_SRC}/ConfigHandler.cpp
    ${WaveShare-stereo-camera_SRC}/CameraCalibrationAssistant.cpp
    ${WaveShare-stereo-camera_SRC}/DepthImage.cpp
)

set(Library_PCH
    ${WaveShare-stereo-camera_PCH}/ProjectHeader.pch
)

find_package(Threads REQUIRED)

add_library(${CMAKE_PROJECT_NAME} ${Library_SRC})
target_precompile_headers(${CMAKE_PROJECT_NAME} PUBLIC ${Library_PCH})
target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${CMAKE_PROJECT_NAME} Threads::Threads)

add_executable(${CMAKE_PROJECT_NAME}-ImageCapturingDemo demoPrograms/ImageCapturingDemo.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}-ImageCapturingDemo ${CMAKE_PROJECT_NAME})

add_executable(${CMAKE_PROJECT_NAME}-CalibrationDemo demoPrograms/CalibrationDemo.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}-CalibrationDemo ${CMAKE_PROJECT_NAME})

add_executable(${CMAKE_PROJECT_NAME}-DepthDemo demoPrograms/DepthDemo.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}-DepthDemo ${CMAKE_PROJECT_NAME})