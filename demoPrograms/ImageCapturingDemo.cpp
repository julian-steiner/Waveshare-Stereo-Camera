#include "StereoCamera.h"
#include <iostream>
#include "opencv2/opencv.hpp"

int main()
{
    waveshare::StereoCamera camera(2, 4);
    waveshare::StereoImage img;

    img = camera.read();

    cv::imshow("image 1", img.image1);
    cv::imshow("image 2", img.image2);
    cv::imshow("stereo image", img.getCombinedImage());

    cv::waitKey(0);

    img.saveToFile("images/testImage.png", false);
}