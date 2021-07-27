#include "CameraCalibrationAssistant.h"
#include <stdlib.h>

using namespace waveshare;

void CalibrationAssistant::loadImages(const cfg::CalibrationConfig& config, std::vector<StereoImage>& images)
{
    std::vector<std::string> files;
    files.reserve(config.numberOfFrames);
    std::stringstream searchDirectory;
    searchDirectory << config.filepathToFrames << "left/";

    // Finding all the filenames in the directory
    cv::glob(searchDirectory.str(), files);

    if (files.size() > config.numberOfFrames)
    {
        std::cerr << "ERROR: To many images in the folder, expected: " << config.numberOfFrames << " recieved: " << files.size() << std::endl;
    }

    // Load in every file as a stereoImage
    for (int i = 0; i < config.numberOfFrames; i++)
    {
        std::string filename = files.at(i);

        images.emplace_back(waveshare::StereoImage());
        std::size_t position = filename.find_last_of("/");
        filename = filename.erase(0, position+1);

        images.back().fromFile(config.filepathToFrames, filename);
    }

    if (images.size() == config.numberOfFrames) {}
    else std::cerr << "There was an error loading the calibrationImages" << std::endl;
}

void CalibrationAssistant::generateDefaultObjectPoint(const cfg::CalibrationConfig& config, std::vector<cv::Point3f>& objp)
{
    // Define the real world Coordinates for points on the Chessboard
    for (int i = 0; i < config.boardWidth; i++)
    {
        for (int a = 0; a < config.boardHeight; a++)
        {
            objp.push_back(cv::Point3f(i * config.squareSize, a * config.squareSize, 0));
        }
    }
}

void CalibrationAssistant::generateCalibrationImages(StereoCamera* camera)
{
    cfg::CalibrationConfig config = cfg::readConfig();

    int counter = 0;

    // Create a video stream 
    while (counter < config.numberOfFrames)
    {
        camera->read().show("Live View");

        int keyCode = cv::waitKey(1);
        // If the user presses ENTER, validate the image
        if (keyCode == 13)
        {
            StereoImage img = camera->read();
            StereoImageChessboardData data;

            findChessboardCorners(img, data, config);

            // If the input is valid, save the image
            if (data.foundLeft && data.foundRight)
            {
                std::stringstream filename;
                filename << counter << ".jpg";

                cv::Mat preview1 = img.image1.clone();
                cv::Mat preview2 = img.image2.clone();

                cv::drawChessboardCorners(preview1, {config.boardWidth, config.boardHeight}, data.cornersLeft, data.foundLeft);
                cv::drawChessboardCorners(preview2, {config.boardWidth, config.boardHeight}, data.cornersRight, data.foundRight);

                cv::imshow("previewLeft", preview1);
                cv::imshow("previewRight", preview2);

                //int keyCode = cv::waitKey(0);

                //if (keyCode == 13)
                {
                    img.saveToFile(config.filepathToFrames, filename.str());
                    std::cout << counter+1 << " images of " << config.numberOfFrames << std::endl;
                    counter++;
                }
            }
        }
    }
    cv::destroyAllWindows();
}

void CalibrationAssistant::findChessboardCorners(const StereoImage& image, StereoImageChessboardData& data, const cfg::CalibrationConfig& config)
{
    const cv::Mat& leftImage = image.image1;
    const cv::Mat& rightImage = image.image2;

    // Generates gray versions of the images to find the corners
    cv::Mat grayLeft;
    cv::Mat grayRight;
    cv::cvtColor(leftImage, grayLeft, cv::COLOR_BGR2GRAY);
    cv::cvtColor(rightImage, grayRight, cv::COLOR_BGR2GRAY);

    // Find the corners on both images
    data.foundLeft = cv::findChessboardCorners(grayLeft,
                                               {config.boardHeight, config.boardWidth},
                                               data.cornersLeft,
                                               cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

    data.foundRight = cv::findChessboardCorners(grayRight,
                                                {config.boardHeight, config.boardWidth},
                                                data.cornersRight,
                                                cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

    // If the corners are found, refine the corner location to get Positions at subPixel accuracy
    if (data.foundLeft && data.foundRight)
    {
        cv::cornerSubPix(grayLeft,
                        data.cornersLeft,
                        cv::Size(11, 11),
                        cv::Size(-1, -1),
                        cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.0001));

        cv::cornerSubPix(grayRight,
                         data.cornersRight,
                         cv::Size(11, 11),
                         cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.0001));
    }
}

void CalibrationAssistant::generateImagePoints(StereoImageImagePoints& imagePoints, StereoImageObjectPoints& objectPoints, std::vector<waveshare::StereoImage>& images, const cfg::CalibrationConfig& config, const std::vector<cv::Point3f>& defaultObjectPoint)
{
    // Go through every frame and grab the object and image points if the images are valid
    for (int i = 0; i < config.numberOfFrames; i++)
    {
        StereoImage& image = images.at(i);
        StereoImageChessboardData data;

        findChessboardCorners(image, data, config);

        if (config.showImagesDuringCalibration)
        {
            cv::Mat preview1, preview2;
            preview1 = image.image1.clone();
            preview2 = image.image2.clone();

            cv::drawChessboardCorners(preview1, {config.boardHeight, config.boardWidth}, data.cornersLeft, data.foundLeft);
            cv::drawChessboardCorners(preview2, {config.boardHeight, config.boardWidth}, data.cornersRight, data.foundRight);

            cv::imshow("PreviewLeft", preview1);
            cv::imshow("PreviewRight", preview2);

            cv::waitKey(500);
        }
        
        if (data.foundLeft && data.foundRight)
        {
            imagePoints.imagePointsLeft.push_back(data.cornersLeft);
            imagePoints.imagePointsRight.push_back(data.cornersRight);

            objectPoints.objectPoints.push_back(defaultObjectPoint);
        }
    }
    cv::destroyAllWindows();
}

double CalibrationAssistant::calibrateIntrinsics(const StereoImageObjectPoints& objectPoints, const std::vector<std::vector<cv::Point2f>>& imagePoints, CameraIntrinsics& intrinsics, CameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags)
{
    double error = cv::calibrateCamera(objectPoints.objectPoints,
                        imagePoints,
                        imageSize, intrinsics.cameraMatrix,
                        intrinsics.distortionCoefficients,
                        extrinsics.rotationVectors,
                        extrinsics.translationVectors,
                        flags);
    std::cout << "CalibrationError: " << error << std::endl;
    return error;
}

void CalibrationAssistant::calibrateStereoCamera(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags, const cv::TermCriteria& criteria)
{
    double error = cv::stereoCalibrate(objectPoints.objectPoints,
                        imagePoints.imagePointsLeft,
                        imagePoints.imagePointsRight,
                        intrinsics.left.cameraMatrix,
                        intrinsics.left.distortionCoefficients,
                        intrinsics.right.cameraMatrix,
                        intrinsics.right.distortionCoefficients,
                        imageSize,
                        extrinsics.rotationVectors,
                        extrinsics.translationVectors,
                        extrinsics.essentialMatrix,
                        extrinsics.fundamentalMatrix,
                        flags,
                        criteria);
    std::cout << "StereoCam CalibrationError: " << error << std::endl;
}

void CalibrationAssistant::computeStereoRectification(StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, StereoCameraRectification& rect, const cv::Size& imageSize)
{
    cv::stereoRectify(intrinsics.left.cameraMatrix,
                      intrinsics.left.distortionCoefficients,
                      intrinsics.right.cameraMatrix,
                      intrinsics.right.distortionCoefficients,
                      imageSize,
                      extrinsics.rotationVectors,
                      extrinsics.translationVectors,
                      rect.left.rect,
                      rect.right.rect,
                      rect.left.projMatrix,
                      rect.right.projMatrix,
                      rect.Q,
                      1024,
                      -1,
                      imageSize,
                      &intrinsics.left.roi,
                      &intrinsics.right.roi);
}

void CalibrationAssistant::computeRectificationMap(const CameraIntrinsics& intrinsics, const CameraRectification& rect, const cv::Size& imageSize, RectificationMap& stereoMap)
{
    cv::initUndistortRectifyMap(intrinsics.cameraMatrix,
                                intrinsics.distortionCoefficients,
                                rect.rect,
                                rect.projMatrix,
                                imageSize,
                                CV_32FC1,
                                stereoMap.mapX,
                                stereoMap.mapY);
}

void CalibrationAssistant::computeStereoMap(const StereoCameraIntrinsics& intrinsics, const StereoCameraRectification& rect, const cv::Size& imageSize, StereoMap& stereoMap)
{
    computeRectificationMap(intrinsics.left, rect.left, imageSize, stereoMap.left);
    computeRectificationMap(intrinsics.right, rect.right, imageSize, stereoMap.right);
}

void CalibrationAssistant::computeNewCameraMatrix(CameraIntrinsics& intrinsics, const cv::Size& imageSize)
{
    intrinsics.newCameraMatrix = cv::getOptimalNewCameraMatrix(intrinsics.cameraMatrix,
                                                               intrinsics.distortionCoefficients,
                                                               imageSize,
                                                               1,
                                                               imageSize,
                                                               &intrinsics.roi);
}

double CalibrationAssistant::calibrateStereoCameraSetup(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags)
{
    // Calibrate both cameras
    double leftError = calibrateIntrinsics(objectPoints, imagePoints.imagePointsLeft, intrinsics.left, extrinsics.left, imageSize, flags);
    double rightError = calibrateIntrinsics(objectPoints, imagePoints.imagePointsRight, intrinsics.right, extrinsics.right, imageSize, flags);

    // Generate new, refined camera matrixes 
    // Choose the calibration of the camera with the lower error for the whole setup because the cameras are the same
    if (leftError <= rightError)
    {
        computeNewCameraMatrix(intrinsics.left, imageSize);
        intrinsics.right = intrinsics.left;
    }
    else
    {
        computeNewCameraMatrix(intrinsics.right, imageSize);
        intrinsics.right = intrinsics.left;
    }

    // Calibrate the camera as a stereo setup
    int stereoFlags = cv::CALIB_FIX_INTRINSIC + cv::CALIB_SAME_FOCAL_LENGTH + cv::CALIB_ZERO_TANGENT_DIST;
    cv::TermCriteria stereoCriteria = cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::MAX_ITER, 30, 1e-6);

    calibrateStereoCamera(objectPoints, imagePoints, intrinsics, extrinsics, imageSize, stereoFlags, stereoCriteria);

    return 0;
}

void CalibrationAssistant::saveCalibrationData(const std::string& outputFilePath, const StereoCameraIntrinsics& intrinsics, const StereoMap& stereoMap, const StereoCameraRectification& rect)
{
    cv::FileStorage storage(outputFilePath, cv::FileStorage::WRITE);

    storage << "Intrinsics" << intrinsics;
    storage << "StereoMap" << stereoMap;
    storage << "Rectification" << rect;

    storage.release();
}

void CalibrationAssistant::computeCalibrationMatrices(const std::string& outputFilePath)
{
    // Variables
    cfg::CalibrationConfig config = cfg::readConfig();
    std::vector<waveshare::StereoImage> images;
    images.reserve(config.numberOfFrames);
    StereoImageObjectPoints objectPoints;
    StereoImageImagePoints imagePoints;
    StereoCameraIntrinsics intrinsics;
    StereoCameraExtrinsics extrinsics;
    StereoMap stereoMap;
    StereoCameraRectification rect;
    std::vector<cv::Point3f> objp;

    //int flags = cv::CALIB_FIX_ASPECT_RATIO + cv::CALIB_FIX_K1 + cv::CALIB_FIX_K2 + cv::CALIB_FIX_K3 + cv::CALIB_FIX_K4 + cv::CALIB_FIX_K5 + cv::CALIB_ZERO_TANGENT_DIST + cv::CALIB_FIX_PRINCIPAL_POINT;
    //int flags =  cv::CALIB_FIX_ASPECT_RATIO + cv::CALIB_FIX_PRINCIPAL_POINT + cv::CALIB_ZERO_TANGENT_DIST + cv::CALIB_FIX_K3;
    int flags = 0;

    loadImages(config, images);

    generateDefaultObjectPoint(config, objp);

    generateImagePoints(imagePoints, objectPoints, images, config, objp);

    // calibrating the camera
    double error = calibrateStereoCameraSetup(objectPoints, imagePoints, intrinsics, extrinsics, images.at(0).image1.size(), flags);

    computeStereoRectification(intrinsics, extrinsics, rect, images.at(0).image1.size());
    
    computeStereoMap(intrinsics, rect, images.at(0).image1.size(), stereoMap);

    saveCalibrationData(outputFilePath, intrinsics, stereoMap, rect);

    cv::Mat testImg = images[0].image1;
    cv::Mat undistorted;

    cv::undistort(testImg, undistorted, intrinsics.left.cameraMatrix, intrinsics.left.distortionCoefficients, intrinsics.left.newCameraMatrix);

    cv::imshow("test", undistorted);

    cv::waitKey(0);
}