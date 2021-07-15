#ifndef CALIBRATION_ASSISTANT
#define CALIBRATION_ASSISTANT

#include "StereoCamera.h"
#include "ConfigHandler.h"

namespace waveshare
{
    class CalibrationAssistant
    {
    private:
        static void findChessboardCorners(const StereoImage&, StereoImageChessboardData& data, const cfg::CalibrationConfig& config);
        static void calibrateIntrinsics(const StereoImageObjectPoints& objectPoints, const std::vector<std::vector<cv::Point2f>>& imagePoints, CameraIntrinsics& intrinsics, CameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags);
        static void calibrateStereoCamera(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags, const cv::TermCriteria& criteria);
        static double calibrateStereoCameraSetup(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags);

    public:
        static void generateCalibrationImages(StereoCamera* camera);
        static void computeCalibrationMatrices();
    };
}

#endif