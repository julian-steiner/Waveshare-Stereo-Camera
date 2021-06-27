#include "StereoCamera.h"

using namespace waveshare;

StereoCamera::StereoCamera(int cameraPort1, int cameraPort2)
{
    camera1 = cv::VideoCapture(cameraPort1);
    camera2 = cv::VideoCapture(cameraPort2);
    
    try
    {
        if (!camera1.isOpened() && !camera2.isOpened())
        {
            std::throw_with_nested<int>(101);
        }

        if (!camera1.isOpened())
        {
            throw 102;
        }

        if (!camera2.isOpened())
        {
            throw 103;
        }
    }

    catch(int errorCode)
    {
        if (errorCode == 101) 
        {
            std::cout << "Camera 1 and 2 are not connected" << std::endl;
        }

        else if (errorCode == 102) 
        {
            std::cout << "Camera 1 is not connected" << std::endl;
        }

        else if (errorCode == 103) 
        {
            std::cout << "Camera 2 is not connected" << std::endl;
        }
    }
}

StereoImage StereoCamera::read()
{
    StereoImage imageBuffer;
    camera1.read(imageBuffer.image1);
    camera2.read(imageBuffer.image2);
    return std::move(imageBuffer);
}
