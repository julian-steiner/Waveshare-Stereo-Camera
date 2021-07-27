#ifndef WAVESHARE_STEREO_CAMERA_BASE
#define WAVESHARE_STEREO_CAMERA_BASE

#include "StereoImage.h"
#include "DepthImage.h"

namespace waveshare
{
    class StereoCamera;
}

namespace waveshare
{
    struct CameraIntrinsics
    {
        cv::Matx33f cameraMatrix = cv::Matx33f::eye();
        cv::Mat newCameraMatrix;
        cv::Mat distortionCoefficients;
        //cv::Mat distortionCoefficients = cv::Mat::zeros(8, 1, CV_64F);

        cv::Rect roi;

        void write(cv::FileStorage& fs) const;

        void read(const cv::FileNode& node);
    };

    static void write(cv::FileStorage& fs, const std::string&, const CameraIntrinsics& data)
    { 
        data.write(fs);
    }
    static void read(const cv::FileNode& node, CameraIntrinsics& data, const CameraIntrinsics& defaultValue)
    {
        if(node.empty()) data = defaultValue;
        else data.read(node);
    }
    static std::ostream& operator<<(std::ostream& out, const CameraIntrinsics& data)
    {
        out << "{ cameraMatrix = " << data.cameraMatrix << ", ";
        out << "newCameraMatrix = " << data.newCameraMatrix << ", ";
        out << "distortionCoefficients = " << data.distortionCoefficients << " }";
        return out;
    }

    struct CameraExtrinsics
    {
        std::vector<cv::Mat> rotationVectors;
        std::vector<cv::Mat> translationVectors;
    };

    struct StereoCameraIntrinsics
    {
        CameraIntrinsics left;
        CameraIntrinsics right;

        void write(cv::FileStorage& fs) const;

        void read(const cv::FileNode& node);
    };

    static void write(cv::FileStorage& fs, const std::string&, const StereoCameraIntrinsics& data)
    { 
        data.write(fs);
    }
    static void read(const cv::FileNode& node, StereoCameraIntrinsics& data, const StereoCameraIntrinsics& defaultValue)
    {
        if(node.empty()) data = defaultValue;
        else data.read(node);
    }
    static std::ostream& operator<<(std::ostream& out, const StereoCameraIntrinsics& data)
    {
        out << "{ left = " << data.left << ", ";
        out << "right = " << data.right << " }";
        return out;
    }

    struct StereoCameraExtrinsics
    {
        CameraExtrinsics left;
        CameraExtrinsics right;

        cv::Mat rotationVectors;
        cv::Mat translationVectors;
        cv::Mat essentialMatrix;
        cv::Mat fundamentalMatrix;
    };

    struct CameraRectification
    {
        cv::Mat rect;

        cv::Mat projMatrix;
    };

    struct StereoCameraRectification
    {
        CameraRectification left;
        CameraRectification right;
        
        cv::Mat Q;

        void write(cv::FileStorage& fs) const;

        void read(const cv::FileNode& node);
    };

    static void write(cv::FileStorage& fs, const std::string&, const StereoCameraRectification& data)
    { 
        data.write(fs);
    }
    static void read(const cv::FileNode& node, StereoCameraRectification& data, const StereoCameraRectification& defaultValue)
    {
        if(node.empty()) data = defaultValue;
        else data.read(node);
    }
    static std::ostream& operator<<(std::ostream& out, const StereoCameraRectification& data)
    {
        out << "{ Q = " << data.Q << " }";
        return out;
    }
}

namespace waveshare
{
    class StereoCamera
    {
        cv::VideoCapture camera1;
        cv::VideoCapture camera2;
        bool calibrated;
        bool videoThreadRunning;
        std::thread videoThread;

        static void videoTask(StereoCamera* camera, const std::string& name);

    public:
        StereoCameraIntrinsics intrinsics;
        StereoMap stereoMap;
        StereoCameraRectification rectification;

        StereoCamera() = default;
        StereoCamera(int cameraPort1, int cameraPort2);

        void setResolution(const ImageSize& size);
        StereoImage read();
        waveshare::DepthImage generateDepthMap();
        void loadCalibrationData(const std::string& filepath);

        void startVideoStream(const std::string&);
        void endVideoStream();
    };
}

#endif