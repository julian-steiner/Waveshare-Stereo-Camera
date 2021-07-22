#include "StereoCamera.h"
#include <iostream>
#include "opencv2/opencv.hpp"

int main()
{
    waveshare::StereoCamera camera(2, 4);
    waveshare::StereoImage img;

    std::cout << "Press ENTER or q to stop the stream and save the last image" << std::endl;

    while (true)
    {
        img = camera.read();

        img.show("separateImage");

        int keyCode = cv::waitKey(1); 
        if (keyCode == 113 || keyCode == 13)
        {
            break;
        }
    }

    //img.saveToFile("images/testImage.png", true);
    //img.saveToFile("images/testImage.png", false);
}