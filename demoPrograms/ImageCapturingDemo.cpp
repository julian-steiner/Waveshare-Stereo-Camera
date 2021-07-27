#include "StereoCamera.h"
#include <iostream>
#include "opencv2/opencv.hpp"

int main()
{
    waveshare::StereoCamera camera(2, 4);
    waveshare::StereoImage img;
    camera.setResolution({1280, 720});

    camera.startVideoStream("Hello World");

    std::cin.get();

    camera.endVideoStream();

    img = camera.read();

    img.saveToFile("images/", "testImage.png");
    img.saveToFile("images/", "testImage.png");

    std::cout << "saved" << std::endl;
}