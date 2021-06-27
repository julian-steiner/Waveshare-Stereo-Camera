#ifndef WAVESHARE_STEREO_CAMERA_BASE
#define WAVESHARE_STEREO_CAMERA_BASE

#include "StereoImage.h"

namespace waveshare
{
    class StereoCamera
    {
        cv::VideoCapture camera1;
        cv::VideoCapture camera2;

    public:
        StereoCamera() = default;
        StereoCamera(int cameraPort1, int cameraPort2);

        StereoImage read();
    };
}

#endif