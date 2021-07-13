#include "CameraCalibrationAssistant.h"

using namespace waveshare;

void CalibrationAssistant::generateCalibrationImages(StereoCamera* camera)
{
    cfg::CalibrationConfig config = cfg::readConfig();

    int counter = 0;

    while (counter < config.numberOfFrames)
    {
        camera->read().show("Preview", false);
        int keyCode = cv::waitKey(1);
        if (keyCode == 13)
        {
            std::stringstream filename;
            filename << config.filepathToFrames << counter << ".jpg";
            std::cout << filename.str() << std::endl;
            camera->read().saveToFile(filename.str(), true);
            counter++;
        }
    }
}

void CalibrationAssistant::calibrateCamera(StereoCamera* camera)
{
}