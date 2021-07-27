#ifndef CALIBRATION_ASSISTANT
#define CALIBRATION_ASSISTANT

#include "StereoCamera.h"
#include "ConfigHandler.h"

namespace waveshare
{
    class CalibrationAssistant
    {
    private:
        static void loadImages(const cfg::CalibrationConfig& config, std::vector<StereoImage>& images);

        static void findChessboardCorners(const StereoImage&, StereoImageChessboardData& data, const cfg::CalibrationConfig& config);
        static void generateDefaultObjectPoint(const cfg::CalibrationConfig& config, std::vector<cv::Point3f>& objp);
        static void generateImagePoints(StereoImageImagePoints& imagePoints, StereoImageObjectPoints& objectPoints, std::vector<waveshare::StereoImage>& images, const cfg::CalibrationConfig& config, const std::vector<cv::Point3f>& defaultObjectPoint);

        static double calibrateStereoCameraSetup(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags);
        static double calibrateIntrinsics(const StereoImageObjectPoints& objectPoints, const std::vector<std::vector<cv::Point2f>>& imagePoints, CameraIntrinsics& intrinsics, CameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags);
        static void computeNewCameraMatrix(CameraIntrinsics& intrinsics, const cv::Size& ImageSize);
        static void calibrateStereoCamera(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags, const cv::TermCriteria& criteria);

        static void computeStereoRectification(StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, StereoCameraRectification& rect, const cv::Size& imageSize);
        static void computeRectificationMap(const CameraIntrinsics& intrinsics, const CameraRectification& rect, const cv::Size& imageSize, RectificationMap& stereoMap);
        static void computeStereoMap(const StereoCameraIntrinsics& intrinsics, const StereoCameraRectification& rect, const cv::Size& imageSize, StereoMap& stereoMap);

        static void saveCalibrationData(const std::string& outputFilePath, const StereoCameraIntrinsics& intrinsics, const StereoMap& stereoMap, const StereoCameraRectification& rect);

    public:
        static void generateCalibrationImages(StereoCamera* camera);
        static void computeCalibrationMatrices(const std::string& outputFilePath);
    };
}

#endif