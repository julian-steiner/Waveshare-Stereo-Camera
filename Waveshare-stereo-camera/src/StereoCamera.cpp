#include "StereoCamera.h"

using namespace waveshare;

StereoCamera::StereoCamera(int cameraPort1, int cameraPort2)
{
    do
    {
        camera1 = cv::VideoCapture(cameraPort1);
        camera2 = cv::VideoCapture(cameraPort2);
    
        try
        {
            if (!camera1.isOpened() && !camera2.isOpened())
            {
                throw 101;
            }

            if (!camera1.isOpened())
            {
                throw 102;
            }

            if (!camera2.isOpened())
            {
                throw 103;
            }
        }

        catch(int errorCode)
        {
            if (errorCode == 101) 
            {
                std::cerr << "Camera 1 and 2 are not connected" << std::endl;
            }

            else if (errorCode == 102) 
            {
                std::cerr << "Camera 1 is not connected" << std::endl;
            }

            else if (errorCode == 103) 
            {
                std::cerr << "Camera 2 is not connected" << std::endl;
            }

            usleep(1000000);
        }
    } while (!camera1.isOpened() || !camera2.isOpened());
}

StereoImage StereoCamera::read()
{
    StereoImage imageBuffer;
    double zoom = 2;

    camera1.read(imageBuffer.image1);
    camera2.read(imageBuffer.image2);

    if (calibrated)
    {
        cv::remap(imageBuffer.image1, imageBuffer.image1, stereoMap.left.mapX, stereoMap.left.mapY, cv::INTER_LANCZOS4, cv::BORDER_CONSTANT, 0);
        cv::remap(imageBuffer.image2, imageBuffer.image2, stereoMap.right.mapX, stereoMap.right.mapY, cv::INTER_LANCZOS4, cv::BORDER_CONSTANT, 0);
    }

    return std::move(imageBuffer);
}

void StereoCamera::setResolution(const ImageSize& size)
{
    camera1.set(3, size.x);
    camera2.set(3, size.x);
    camera1.set(4, size.y);
    camera2.set(4, size.y);
}

void StereoCamera::loadCalibrationData(const std::string& filepath)
{
    cv::FileStorage storage(filepath, cv::FileStorage::READ);

    storage["Intrinsics"] >> intrinsics;
    storage["StereoMap"] >> stereoMap;
    storage["Rectification"] >> rectification;

    storage.release();

    calibrated = true;
}

waveshare::DepthImage StereoCamera::generateDepthMap()
{
    StereoImage image = read();
    return std::move(DepthImage::computeFromStereoImage(image, 8000, 2));
}

void StereoCamera::videoTask(StereoCamera* camera, const std::string& name)
{
    while (true && camera->videoThreadRunning)
    {
        waveshare::StereoImage img = camera->read();

        img.show(name);

        cv::waitKey(1);
    }
}

void StereoCamera::startVideoStream(const std::string& name)
{
    videoThreadRunning = true;
    videoThread = std::thread(videoTask, this, name);
}

void StereoCamera::endVideoStream()
{
    videoThreadRunning = false;
    videoThread.join();
}

void CameraIntrinsics::write(cv::FileStorage& fs) const
{
    fs << "{" << "cameraMatrix" << cameraMatrix << "newCameraMatrix" << newCameraMatrix << "distortionCoefficients" << distortionCoefficients << "}";
}

void CameraIntrinsics::read(const cv::FileNode& node)
{
    node["cameraMatrix"] >> cameraMatrix;
    node["newCameraMatrix"] >> newCameraMatrix;
    node["distortionCoefficients"] >> distortionCoefficients;
}

void StereoCameraIntrinsics::write(cv::FileStorage& fs) const
{
    fs << "{" << "left" << left << "right" << right << "}";
}

void StereoCameraIntrinsics::read(const cv::FileNode& node)
{
    node["left"] >> left;
    node["right"] >> right;
}

void StereoCameraRectification::write(cv::FileStorage& fs) const
{
    fs << "{" << "Q" << Q << "}";
}

void StereoCameraRectification::read(const cv::FileNode& node)
{
    node["Q"] >> Q;
}
