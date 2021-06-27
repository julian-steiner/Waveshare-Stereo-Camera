#include "StereoImage.h"

using namespace waveshare;

StereoImage::StereoImage(StereoImage&& other)
{
    cv::Mat* image1Pointer = &image1;
    cv::Mat* image2Pointer = &image2;

    *image1Pointer = other.image1;
    *image2Pointer = other.image2;
}

cv::Mat StereoImage::getCombinedImage()
{
    return (image1 + image2) / 2;
}

void StereoImage::saveToFile(const std::string& filename, const bool& seperateFiles)
{
    if (seperateFiles)
    {
        cv::imwrite(filename.substr(0, filename.find(".")) + "_left" + filename.substr(filename.find(".")), image1);
        cv::imwrite(filename.substr(0, filename.find(".")) + "_right" + filename.substr(filename.find(".")), image2);
    }

    else
    {
        cv::imwrite(filename, image1 + image2);
    }
}