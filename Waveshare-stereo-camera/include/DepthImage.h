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

        void show(const std::string& windowname);
        void saveToFile(const std::string& folder, const std::string& filename);
        void fromFile(const std::string& folder, const std::string& filename);
        void visualize(int scale = 10);
        void colorize(cv::ColormapTypes colorCode);
    };
}

#endif