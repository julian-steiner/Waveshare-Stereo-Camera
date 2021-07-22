#include "ConfigHandler.h"

using namespace cfg;

void CalibrationConfig::write(cv::FileStorage& fs) const
{
    fs << "{" << "numberOfFrames" << numberOfFrames << "boardWidth" << boardWidth << "boardHeight" << boardHeight << "squareSize" << squareSize << "showImagesDuringCalibration" << showImagesDuringCalibration << "outputFilename" << outputFilename << "filepathToFrames" << filepathToFrames << "}";
}

void CalibrationConfig::read(const cv::FileNode& node)
{
    numberOfFrames = (int)node["numberOfFrames"];
    boardHeight = (int)node["boardHeight"];
    boardWidth = (int)node["boardWidth"];
    squareSize = (int)node["squareSize"];
    node["showImagesDuringCalibration"] >> showImagesDuringCalibration;
    outputFilename = (std::string)node["outputFilename"];
    filepathToFrames = (std::string)node["filepathToFrames"];
}