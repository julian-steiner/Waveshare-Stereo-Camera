#ifndef CALIBRATION_ASSISTANT
#define CALIBRATION_ASSISTANT

#include "StereoCamera.h"
#include "ConfigHandler.h"

namespace waveshare
{
    struct StereoImageChessboardData
    {
        bool foundRight;
        bool foundLeft;

        std::vector<cv::Point2f> cornersRight;
        std::vector<cv::Point2f> cornersLeft;
    };

    struct StereoImageObjectPoints
    {
        std::vector<std::vector<cv::Point3f>> objectPointsLeft;
        std::vector<std::vector<cv::Point3f>> objectPointsRight;
    };
    
    struct StereoImageImagePoints
    {
        std::vector<std::vector<cv::Point2f>> imagePointsLeft;
        std::vector<std::vector<cv::Point2f>> imagePointsRight;
    };

    struct StereoCameraIntrinsics
    {
        cv::Matx33f cameraMatrixLeft = cv::Matx33f::eye();
        cv::Mat distortionCoefficientsLeft = cv::Mat::zeros(8, 1, CV_64F);

        cv::Matx33f cameraMatrixRight = cv::Matx33f::eye();
        cv::Mat distortionCoefficientsRight = cv::Mat::zeros(8, 1, CV_64F);
    };

    struct StereoCameraExtrinsics
    {
        std::vector<cv::Mat> rotationVectorsLeft;
        std::vector<cv::Mat> translationVectorsLeft;

        std::vector<cv::Mat> rotationVectorsRight;
        std::vector<cv::Mat> translationVectorsRight;
    };

    struct StereoCameraLensInterpolation
    {
        cv::Mat mapXLeft, mapXRight;
        cv::Mat mapYLeft, mapYRight;
    };

    class CalibrationAssistant
    {
    private:
        static void findChessboardCorners(const StereoImage&, StereoImageChessboardData& data, const cfg::CalibrationConfig& config);
        static double calibrateCamera(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags);
        static void computeLensInterpolation(const StereoCameraIntrinsics& intrinstics, const cv::Size& imageSize, StereoCameraLensInterpolation& interpolation);

    public:
        static void generateCalibrationImages(StereoCamera* camera);
        static void computeCalibrationMatrices();
    };
}

#endif