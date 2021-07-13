#ifndef CALIBRATION_ASSISTANT
#define CALIBRATION_ASSISTANT

#include "StereoCamera.h"
#include "ConfigHandler.h"

namespace waveshare
{
    namespace CalibrationAssistant
    {
        void generateCalibrationImages(StereoCamera* camera);
        void calibrateCamera(StereoCamera* camera);
    };
}

#endif