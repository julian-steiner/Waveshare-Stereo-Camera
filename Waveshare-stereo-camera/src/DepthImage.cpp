#include "DepthImage.h"

using namespace waveshare;

void DepthImage::show(const std::string& windowname)
{
    if (!visualized)
    {
        visualize();
    }
    cv::imshow(windowname, disparityMap);
}

void DepthImage::saveToFile(const std::string& folder, const std::string& filename)
{
    std::stringstream filepath;
    filepath << folder << filename;

    cv::imwrite(filepath.str(), disparityMap);
}
        
void DepthImage::fromFile(const std::string& folder, const std::string& filename)
{
    std::stringstream filepath;
    filepath << folder << filename;

    disparityMap = cv::imread(filepath.str());
}

void DepthImage::visualize(int scale)
{
    cv::ximgproc::getDisparityVis(disparityMap, disparityMap, 10);
    visualized = true;
}

void DepthImage::colorize(cv::ColormapTypes colorCode)
{
    if (visualized) cv::applyColorMap(disparityMap, disparityMap, colorCode);

    else std::cerr << "Can only colorize visualized images" << std::endl;
}