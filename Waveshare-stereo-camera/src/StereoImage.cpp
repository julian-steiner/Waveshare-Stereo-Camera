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

void StereoImage::saveToFile(const std::string& folder, const std::string& filename, const bool& combined)
{
    if (!combined)
    {
        std::stringstream filepath1;
        std::stringstream filepath2;

        filepath1 << folder << "left/" << filename;
        filepath2 << folder << "right/" << filename;

        cv::imwrite(filepath1.str(), image1);
        cv::imwrite(filepath2.str(), image2);
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

void StereoImage::fromFile(const std::string& folder, const std::string& filename)
{
    try
    {
        std::stringstream filepath1;
        std::stringstream filepath2;

        filepath1 << folder << "left/" << filename;
        filepath2 << folder << "right/" << filename;

        image1 = cv::imread(filepath1.str());
        image2 = cv::imread(filepath2.str());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void StereoImage::rectifySingleImage(cv::Mat& image, const RectificationMap& rectMap)
{
        cv::remap(image,
                  image,
                  rectMap.mapX,
                  rectMap.mapY,
                  cv::INTER_LANCZOS4,
                  cv::BORDER_CONSTANT,
                  0);
}

void StereoImage::rectify(const StereoMap& stereoMap)
{
    rectifySingleImage(image1, stereoMap.left);
    rectifySingleImage(image2, stereoMap.right);
}

void RectificationMap::write(cv::FileStorage& fs) const
{
    fs << "{" << "mapX" << mapX << "mapY" << mapY << "}";
}

void RectificationMap::read(const cv::FileNode& node) 
{
    node["mapX"] >> mapX;
    node["mapY"] >> mapY;
}

void StereoMap::write(cv::FileStorage& fs) const
{
    fs << "{" << "left" << left << "right" << right << "}";
}

void StereoMap::read(const cv::FileNode& node)
{
    node["left"] >> left;
    node["right"] >> right;
}
