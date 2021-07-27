#include "StereoCamera.h"
#include "ConfigHandler.h"
#include "opencv4/opencv2/opencv.hpp"

int main()
{
    waveshare::StereoCamera camera(2, 4);
    camera.setResolution({1280, 720});
    camera.loadCalibrationData("CalibrationData.xml");

    waveshare::DepthImage depth;
    waveshare::StereoImage color;

    while (true)
    {
        depth = camera.generateDepthMap();
        depth.visualize(20);
        //depth.colorize(cv::ColormapTypes::COLORMAP_BONE);
        depth.colorize(cv::ColormapTypes::COLORMAP_JET);
        depth.show("DepthMap");

        int keyCode = cv::waitKey(1);

        if (keyCode == 13)
        {
            break;
        }
    }

    depth.saveToFile("", "Depth.jpg");
}

