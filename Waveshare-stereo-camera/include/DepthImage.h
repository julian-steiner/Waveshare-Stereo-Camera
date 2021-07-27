#ifndef DEPTHIMAGE
#define DEPTHIMAGE

#include "StereoImage.h"

namespace waveshare
{
    class DepthImage : public Image
    {
    private:
        bool visualized;

    public:
        cv::Mat disparityMap;

        static DepthImage computeFromStereoImage(const StereoImage& stereoImage, int lambda = 8000, double sigma = 2);

        void show(const std::string& windowname);
        void saveToFile(const std::string& folder, const std::string& filename);
        void fromFile(const std::string& folder, const std::string& filename);
        void visualize(int scale = 10);
        void colorize(cv::ColormapTypes colorCode);
    };
}

#endif