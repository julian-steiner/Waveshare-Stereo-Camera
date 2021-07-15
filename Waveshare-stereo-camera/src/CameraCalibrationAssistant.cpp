#include "CameraCalibrationAssistant.h"

using namespace waveshare;

void CalibrationAssistant::generateCalibrationImages(StereoCamera* camera)
{
    cfg::CalibrationConfig config = cfg::readConfig();

    int counter = 0;

    // Create a video stream 
    while (counter < config.numberOfFrames)
    {
        camera->read().show("Live View", false);
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
                filename << config.filepathToFrames << counter << ".jpg";
                std::cout << filename.str() << std::endl;
                img.saveToFile(filename.str(), false);
                counter++;
            }
        }
    }
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
                                               {config.boardWidth, config.boardHeight},
                                               data.cornersLeft,
                                               cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

    data.foundRight = cv::findChessboardCorners(grayRight,
                                                {config.boardWidth, config.boardHeight},
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
                         data.cornersLeft,
                         cv::Size(11, 11),
                         cv::Size(-1, -1),
                         cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.0001));
    }
}

void CalibrationAssistant::calibrateIntrinsics(const StereoImageObjectPoints& objectPoints, const std::vector<std::vector<cv::Point2f>>& imagePoints, CameraIntrinsics& intrinsics, CameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags)
{
    cv::calibrateCamera(objectPoints.objectPoints,
                        imagePoints,
                        imageSize, intrinsics.cameraMatrix,
                        intrinsics.distortionCoefficients,
                        extrinsics.rotationVectors,
                        extrinsics.translationVectors,
                        flags);
}

void CalibrationAssistant::calibrateStereoCamera(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags, const cv::TermCriteria& criteria)
{
    cv::stereoCalibrate(objectPoints.objectPoints,
                        imagePoints.imagePointsLeft,
                        imagePoints.imagePointsRight,
                        intrinsics.left.newCameraMatrix,
                        intrinsics.left.distortionCoefficients,
                        intrinsics.right.newCameraMatrix,
                        intrinsics.right.distortionCoefficients,
                        imageSize,
                        extrinsics.rotationVectors,
                        extrinsics.translationVectors,
                        extrinsics.essentialMatrix,
                        extrinsics.fundamentalMatrix,
                        flags,
                        criteria);
}

double CalibrationAssistant::calibrateStereoCameraSetup(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags)
{
    // Calibrate both cameras

    calibrateIntrinsics(objectPoints, imagePoints.imagePointsLeft, intrinsics.left, extrinsics.left, imageSize, flags);
    calibrateIntrinsics(objectPoints, imagePoints.imagePointsRight, intrinsics.right, extrinsics.right, imageSize, flags);
                        
    // Generate new, refined camera matrixes 
    intrinsics.right.newCameraMatrix = cv::getOptimalNewCameraMatrix(intrinsics.right.cameraMatrix,
                                                                    intrinsics.right.distortionCoefficients,
                                                                    imageSize,
                                                                    1,
                                                                    imageSize,
                                                                    &intrinsics.right.roi);

    intrinsics.left.newCameraMatrix = cv::getOptimalNewCameraMatrix(intrinsics.left.cameraMatrix,
                                                                    intrinsics.left.distortionCoefficients,
                                                                    imageSize,
                                                                    1,
                                                                    imageSize, 
                                                                    &intrinsics.left.roi);

    // Calibrate the camera as a stereo setup
    int stereoFlags = 0;
    stereoFlags |= cv::CALIB_FIX_INTRINSIC;
    cv::TermCriteria stereoCriteria = cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::MAX_ITER, 30, 1e-6);

    calibrateStereoCamera(objectPoints, imagePoints, intrinsics, extrinsics, imageSize, stereoFlags, stereoCriteria);

    return 0;
}

void CalibrationAssistant::computeCalibrationMatrices()
{
    cfg::CalibrationConfig config = cfg::readConfig();

    std::vector<waveshare::StereoImage> images;
    images.reserve(config.numberOfFrames);

    // Load in all the images from the folder
    for (int i = 0; i < config.numberOfFrames; i++)
    {
        std::stringstream filepath; 
        filepath << config.filepathToFrames << i << ".jpg";

        images.emplace_back(waveshare::StereoImage());
        images.back().fromFile(filepath.str());
    }

    if (images.size() == config.numberOfFrames) {}
    else std::cerr << "There was an error loading the calibrationImages" << std::endl;

    // Create arrays to store the points
    StereoImageObjectPoints objectPoints;
    StereoImageImagePoints imagePoints;

    // Define the real world Coordinates for points on the Chessboard
    std::vector<cv::Point3f> objp;
    for (int i = 0; i < config.boardHeight; i++)
    {
        for (int a = 0; a < config.boardWidth; a++)
        {
            objp.push_back(cv::Point3f(a * config.squareSize, i * config.squareSize, 0));
        }
    }

    // Go through every frame and grab the object and image points if the images are valid
    for (int i = 0; i < config.numberOfFrames; i++)
    {
        StereoImage& image = images.at(i);
        StereoImageChessboardData data;

        findChessboardCorners(image, data, config);
        
        if (data.foundLeft && data.foundRight)
        {
            imagePoints.imagePointsLeft.push_back(data.cornersLeft);
            imagePoints.imagePointsRight.push_back(data.cornersRight);

            objectPoints.objectPoints.push_back(objp);

            cv::drawChessboardCorners(image.image1, {config.boardHeight, config.boardWidth}, cv::Mat(data.cornersLeft), data.foundLeft);
            cv::drawChessboardCorners(image.image2, {config.boardHeight, config.boardWidth}, cv::Mat(data.cornersRight), data.foundRight);
        }
    }

    // calibrating the camera
    StereoCameraIntrinsics intrinsics;
    StereoCameraExtrinsics extrinsics;

    int flags = cv::CALIB_FIX_ASPECT_RATIO + cv::CALIB_FIX_K1 + cv::CALIB_FIX_K2 + cv::CALIB_FIX_K3 + cv::CALIB_FIX_K4 + cv::CALIB_FIX_K5 + cv::CALIB_ZERO_TANGENT_DIST + cv::CALIB_FIX_PRINCIPAL_POINT;

    double error = calibrateStereoCameraSetup(objectPoints, imagePoints, intrinsics, extrinsics, images.at(0).image1.size(), flags);

    std::cout << "Error: " << error << std::endl;


    // computing lens distortion
    StereoMap interpolation;
    StereoCameraRectification rect;

    cv::stereoRectify(intrinsics.left.newCameraMatrix,
                      intrinsics.left.distortionCoefficients,
                      intrinsics.right.newCameraMatrix,
                      intrinsics.right.distortionCoefficients,
                      images.at(0).image1.size(),
                      extrinsics.rotationVectors,
                      extrinsics.translationVectors,
                      rect.rectLeft,
                      rect.rectRight,
                      rect.projMatrixLeft,
                      rect.projMatrixRight,
                      rect.Q,
                      1024,
                      -1.0,
                      images.at(0).image1.size(),
                      &intrinsics.left.roi,
                      &intrinsics.right.roi);

    cv::initUndistortRectifyMap(intrinsics.left.newCameraMatrix,
                                intrinsics.left.distortionCoefficients,
                                rect.rectLeft,
                                //cv::Matx33f::eye(),
                                rect.projMatrixLeft,
                                //intrinsics.newCameraMatrixLeft,
                                images.at(0).image1.size(),
                                CV_32FC1,
                                interpolation.mapXLeft,
                                interpolation.mapYLeft);

    cv::initUndistortRectifyMap(intrinsics.right.newCameraMatrix,
                                intrinsics.right.distortionCoefficients,
                                rect.rectRight,
                                //cv::Matx33f::eye(),
                                rect.projMatrixRight,
                                //intrinsics.newCameraMatrixRight,
                                images.at(0).image1.size(),
                                CV_32FC1,
                                interpolation.mapXRight, 
                                interpolation.mapYRight);

    for (waveshare::StereoImage image : images)
    {
        cv::Mat undistorted;
        cv::Mat undistorted2;
        cv::Mat img = image.image1;
        cv::Mat img2 = image.image2;

        cv::remap(img,
                  undistorted,
                  interpolation.mapXLeft,
                  interpolation.mapYLeft,
                  cv::INTER_LANCZOS4,
                  cv::BORDER_CONSTANT,
                  0);

        cv::remap(img2,
                  undistorted2,
                  interpolation.mapXRight,
                  interpolation.mapYRight,
                  cv::INTER_LANCZOS4,
                  cv::BORDER_CONSTANT,
                  0);

        cv::imshow("Rectified 1", undistorted);
        cv::imshow("Rectified 2", undistorted2);

        cv::waitKey(0);
    }
}