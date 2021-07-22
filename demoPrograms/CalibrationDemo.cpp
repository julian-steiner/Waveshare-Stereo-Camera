#include "ConfigHandler.h"
#include "StereoCamera.h"
#include "CameraCalibrationAssistant.h"

#include <iostream>

int main()
{
    waveshare::StereoCamera camera(2, 4);
    camera.setResolution({1280, 720});

    std::cout << "Press 1 if you want to load a default config and create the images for calibration" << std::endl;
    std::cout << "Press 2 to just start the live window" << std::endl;
    int keyCode = std::cin.get();
    
    if (keyCode == 49)
    {
        cfg::createDefaultConfig();

        waveshare::CalibrationAssistant::generateCalibrationImages(&camera);
    }

    if (keyCode == 50)
    {
        camera.loadCalibrationData("CalibrationData.xml");

        while (true)
        {
            camera.read().show("live images");
            cv::waitKey(1);
        }
    }

    waveshare::CalibrationAssistant::computeCalibrationMatrices("CalibrationData.xml");
}