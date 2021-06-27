#include "StereoImage.h"

using namespace waveshare;

StereoImage::StereoImage(StereoImage&& other)
{
    cv::Mat* image1Pointer = &image1;
    cv::Mat* image2Pointer = &image2;

    *image1Pointer = other.image1;
    *image2Pointer = other.image2;
}