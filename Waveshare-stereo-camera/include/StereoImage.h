#ifndef STEREO_IMAGE
#define STEREO_IMAGE

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
        std::vector<std::vector<cv::Point3f>> objectPoints;
    };
    
    struct StereoImageImagePoints
    {
        std::vector<std::vector<cv::Point2f>> imagePointsLeft;
        std::vector<std::vector<cv::Point2f>> imagePointsRight;
    };

    struct ImageSize
    {
        int x;
        int y;
    };

    struct StereoImage
    {
        cv::Mat image1;
        cv::Mat image2;

        StereoImage() = default;
        StereoImage(ImageSize size);
        StereoImage(StereoImage&) = default;
        StereoImage(StereoImage&&);
        StereoImage& operator=(const StereoImage&) = default;

        cv::Mat getCombinedImage();
        void show(const std::string& windowname, const bool& combined = false);
        void saveToFile(const std::string& filename, const bool& combined = false);
        void fromFile(const std::string& filepath);
    };
}

#endif