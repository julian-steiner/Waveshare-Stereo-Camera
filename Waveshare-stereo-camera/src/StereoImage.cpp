#include "StereoImage.h"

using namespace waveshare;

StereoImage::StereoImage(ImageSize size)
{
   cv::Mat image1 = cv::Mat({size.x, size.y});
   cv::Mat image2 = cv::Mat({size.x, size.y});
}

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

void StereoImage::saveToFile(const std::string& filename, const bool& combined)
{
    if (!combined)
    {
        cv::imwrite(filename.substr(0, filename.find(".")) + "_left" + filename.substr(filename.find(".")), image1);
        cv::imwrite(filename.substr(0, filename.find(".")) + "_right" + filename.substr(filename.find(".")), image2);
    }

    else
    {
        cv::imwrite(filename, getCombinedImage());
    }
}

void StereoImage::show(const std::string& windowname, const bool& combined)
{
    if (combined)   cv::imshow(windowname, getCombinedImage());

    else
    {
        std::stringstream windowname1;
        std::stringstream windowname2;
        windowname1 << windowname << "_left";
        windowname2 << windowname << "_right";

        cv::imshow(windowname1.str(), image1);
        cv::imshow(windowname2.str(), image2);
    }
}

void StereoImage::fromFile(const std::string& filepath)
{
    try
    {
        image1 = cv::imread(filepath.substr(0, filepath.find(".")) + "_left" + filepath.substr(filepath.find(".")));
        image2 = cv::imread(filepath.substr(0, filepath.find(".")) + "_right" + filepath.substr(filepath.find(".")));
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}