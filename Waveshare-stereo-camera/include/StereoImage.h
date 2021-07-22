#ifndef STEREO_IMAGE
#define STEREO_IMAGE

#include "Image.h"

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

    struct RectificationMap
    {
        cv::Mat mapX;
        cv::Mat mapY;

        void write(cv::FileStorage& fs) const;
        void read(const cv::FileNode& node);
    };

    static void write(cv::FileStorage& fs, const std::string&, const RectificationMap& data)
    { 
        data.write(fs);
    }
    static void read(const cv::FileNode& node, RectificationMap& data, const RectificationMap& defaultValue)
    {
        if(node.empty()) data = defaultValue;
        else data.read(node);
    }
    static std::ostream& operator<<(std::ostream& out, const RectificationMap& data)
    {
        out << "{ mapX = " << data.mapX << ", ";
        out << "mapY = " << data.mapY << " }";
        return out;
    }

    struct StereoMap
    {
        RectificationMap left;
        RectificationMap right;

        void write(cv::FileStorage& fs) const;
        void read(const cv::FileNode& node);
    };

    static void write(cv::FileStorage& fs, const std::string&, const StereoMap& data)
    { 
        data.write(fs);
    }
    static void read(const cv::FileNode& node, StereoMap& data, const StereoMap& defaultValue)
    {
        if(node.empty()) data = defaultValue;
        else data.read(node);
    }
    static std::ostream& operator<<(std::ostream& out, const StereoMap& data)
    {
        out << "{ left = " << data.left << ", ";
        out << "right = " << data.right << " }";
        return out;
    }

    struct ImageSize
    {
        int x;
        int y;
    };
}

namespace waveshare
{
    class StereoImage : public Image
    {
    private:
        void rectifySingleImage(cv::Mat& image, const RectificationMap& rectMap);

    public:
        cv::Mat image1;
        cv::Mat image2;

        StereoImage() = default;
        StereoImage(ImageSize size);
        StereoImage(StereoImage&) = default;
        StereoImage(StereoImage&&);
        StereoImage& operator=(const StereoImage&) = default;

        cv::Mat getCombinedImage();
        void show(const std::string& windowname);
        void saveToFile(const std::string& folder, const std::string& filename);
        void fromFile(const std::string& folder, const std::string& filename);
        void rectify(const StereoMap& stereoMap);
    };
}


#endif