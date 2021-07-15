#ifndef WAVESHARE_STEREO_CAMERA_BASE
#define WAVESHARE_STEREO_CAMERA_BASE

#include "StereoImage.h"

namespace waveshare
{
    class StereoCamera;
}

namespace waveshare
{
    struct CameraIntrinsics
    {
        cv::Mat cameraMatrix;
        cv::Mat newCameraMatrix;
        cv::Mat distortionCoefficients = cv::Mat::zeros(8, 1, CV_64F);

        cv::Rect roi;
    };

    struct CameraExtrinsics
    {
        std::vector<cv::Mat> rotationVectors;
        std::vector<cv::Mat> translationVectors;
    };

    struct StereoCameraIntrinsics
    {
        CameraIntrinsics left;
        CameraIntrinsics right;
    };

    struct StereoCameraExtrinsics
    {
        CameraExtrinsics left;
        CameraExtrinsics right;

        cv::Mat rotationVectors;
        cv::Mat translationVectors;
        cv::Mat essentialMatrix;
        cv::Mat fundamentalMatrix;
    };

    struct StereoCameraRectification
    {
        cv::Mat rectLeft;
        cv::Mat rectRight;

        cv::Mat projMatrixLeft;
        cv::Mat projMatrixRight;

        cv::Mat Q;
    };

    struct StereoMap
    {
        cv::Mat mapXLeft, mapXRight;
        cv::Mat mapYLeft, mapYRight;
    };
}

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