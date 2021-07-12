#include "ConfigHandler.h"

using namespace cfg;

void CalibrationConfig::write(cv::FileStorage& fs) const
{
    fs << "{" << "numberOfFrames" << numberOfFrames << "outputFilename" << outputFilename << "filepathToFrames" << filepathToFrames << "}";
}

void CalibrationConfig::read(const cv::FileNode& node)
{
    numberOfFrames = (int)node["numberOfFrames"];
    outputFilename = (std::string)node["outputFilename"];
    filepathToFrames = (std::string)node["filepathToFrames"];
}

void ConfigHandler::createDefaultConfig()
{
    cv::FileStorage storage("CalibrationConfig.xml", cv::FileStorage::WRITE);
    
    CalibrationConfig config(1);

    storage << "Config" << config;

    storage.release();
}

CalibrationConfig ConfigHandler::readConfig()
{
    cv::FileStorage storage("CalibrationConfig.xml", cv::FileStorage::READ);

    if (!storage.isOpened())
    {
        std::cerr << "Failed to open the config file! FAIL" << std::endl;
    }

    CalibrationConfig config;

    storage["Config"] >> config;

    return config;
}