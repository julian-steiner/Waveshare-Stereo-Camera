#ifndef STEREO_IMAGE
#define STEREO_IMAGE

namespace waveshare
{
    struct StereoImage
    {
        cv::Mat image1;
        cv::Mat image2;

        StereoImage() = default;
        StereoImage(StereoImage&) = default;
        StereoImage(StereoImage&&);
        StereoImage& operator=(StereoImage&&) = default;

        cv::Mat getCombinedImage();
        void saveToFile(const std::string& filename, const bool& seperateFiles);
    };
}

#endif