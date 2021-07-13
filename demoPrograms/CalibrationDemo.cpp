#include "ConfigHandler.h"
#include "StereoCamera.h"

#include <iostream>

int main()
{
    waveshare::StereoCamera camera(2, 4);
    cfg::createDefaultConfig();

    std::cout << "created Config" << std::endl;

    camera.generateCalibrationImages();
}