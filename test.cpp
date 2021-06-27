#include "Waveshare-stereo-camera/include/StereoCamera.h"
#include <iostream>
#include "opencv2/opencv.hpp"

int main()
{
    waveshare::StereoCamera camera(2, 4);

    waveshare::StereoImage img = camera.read();

    cv::imshow("image 1", img.image1);
    cv::imshow("image 2", img.image2);

    cv::waitKey(0);
}