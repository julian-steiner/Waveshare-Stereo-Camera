#ifndef STEREO_IMAGE
#define STEREO_IMAGE

namespace waveshare
{
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