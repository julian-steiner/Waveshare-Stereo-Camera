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

DepthImage DepthImage::computeFromStereoImage(const StereoImage& image, int lambda, double sigma)
{
    StereoImage disparity;
    StereoImage grayscale;
    DepthImage depthMap;

    // Initializing the stereoMatchers and filters
    cv::Ptr<cv::StereoBM> leftMatcher = cv::StereoBM::create(16, 5);
    cv::Ptr<cv::StereoMatcher> rightMatcher = cv::ximgproc::createRightMatcher(leftMatcher);

    cv::Ptr<cv::ximgproc::DisparityWLSFilter> wlsFilter = cv::ximgproc::createDisparityWLSFilter(leftMatcher);
    wlsFilter->setLambda(lambda);
    wlsFilter->setSigmaColor(sigma);

    // Coloring the image Grayscale for easier matching
    cv::cvtColor(image.image1, grayscale.image1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(image.image2, grayscale.image2, cv::COLOR_BGR2GRAY);

    // Computing the left and right depth
    leftMatcher->compute(grayscale.image1, grayscale.image2, disparity.image1);
    rightMatcher->compute(grayscale.image2, grayscale.image1, disparity.image2);

    wlsFilter->filter(disparity.image1, image.image1, depthMap.disparityMap, disparity.image2);

    return std::move(depthMap);
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