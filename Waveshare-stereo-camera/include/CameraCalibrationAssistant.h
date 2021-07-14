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
        cv::Mat cameraMatrixLeft;
        cv::Mat newCameraMatrixLeft;
        cv::Mat distortionCoefficientsLeft = cv::Mat::zeros(8, 1, CV_64F);
        cv::Rect roiLeft;

        cv::Matx33f cameraMatrixRight = cv::Matx33f::eye();
        cv::Matx33f newCameraMatrixRight = cv::Matx33f::eye();
        cv::Mat distortionCoefficientsRight = cv::Mat::zeros(8, 1, CV_64F);
        cv::Rect roiRight;
    };

    struct StereoCameraExtrinsics
    {
        std::vector<cv::Mat> rotationVectorsLeft;
        std::vector<cv::Mat> translationVectorsLeft;

        std::vector<cv::Mat> rotationVectorsRight;
        std::vector<cv::Mat> translationVectorsRight;

        cv::Mat rotationVectors;
        cv::Mat translationVectors;
        cv::Mat essentialMatrix;
        cv::Mat fundamentalMatrix;
    };

    struct StereoCameraLensInterpolation
    {
        cv::Mat mapXLeft, mapXRight;
        cv::Mat mapYLeft, mapYRight;
    };

    struct StereoCameraRectification
    {
        cv::Mat rectLeft;
        cv::Mat rectRight;

        cv::Mat projMatrixLeft;
        cv::Mat projMatrixRight;

        cv::Mat Q;

        cv::Mat stereoMapLeft;
        cv::Mat stereoMapRight;
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