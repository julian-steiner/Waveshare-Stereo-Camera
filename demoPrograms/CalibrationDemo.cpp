#include "ConfigHandler.h"
#include "StereoCamera.h"
#include "CameraCalibrationAssistant.h"

#include <iostream>

int main()
{
    std::cout << "Press 1 if you want to load a default config and create the images for calibration" << std::endl;
    int keyCode = std::cin.get();
    
    if (keyCode == 49)
    {
        waveshare::StereoCamera camera(2, 4);
        camera.setResolution({1280, 720});
        cfg::createDefaultConfig();

        waveshare::CalibrationAssistant::generateCalibrationImages(&camera);
    }

    waveshare::CalibrationAssistant::computeCalibrationMatrices("CalibrationData.xml");
}