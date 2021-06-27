#ifndef STEREO_IMAGE
#define STEREO_IMAGE

namespace waveshare
{
    struct StereoImage
    {
        cv::Mat image1;
        cv::Mat image2;

        StereoImage() = default;
        StereoImage(StereoImage&&);
    };
}

#endif