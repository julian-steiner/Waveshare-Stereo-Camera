#include "CameraCalibrationAssistant.h"

using namespace waveshare;

void CalibrationAssistant::generateCalibrationImages(StereoCamera* camera)
{
    cfg::CalibrationConfig config = cfg::readConfig();

    int counter = 0;

    while (counter < config.numberOfFrames)
    {
        camera->read().show("Live View", false);
        int keyCode = cv::waitKey(1);
        if (keyCode == 13)
        {
            StereoImage img = camera->read();
            StereoImageChessboardData data;

            findChessboardCorners(img, data, config);

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

    cv::Mat grayLeft;
    cv::Mat grayRight;

    cv::cvtColor(leftImage, grayLeft, cv::COLOR_BGR2GRAY);
    cv::cvtColor(rightImage, grayRight, cv::COLOR_BGR2GRAY);

    data.foundLeft = cv::findChessboardCorners(grayLeft, {config.boardHeight, config.boardWidth}, data.cornersLeft, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
    data.foundRight = cv::findChessboardCorners(grayRight, {config.boardHeight, config.boardWidth}, data.cornersRight, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

    if (data.foundLeft)
    {
        cv::cornerSubPix(grayLeft, data.cornersLeft, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.0001));
    }
    if (data.foundRight)
    {
        cv::cornerSubPix(grayRight, data.cornersLeft, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.0001));
    }
}

double CalibrationAssistant::calibrateCamera(const StereoImageObjectPoints& objectPoints, const StereoImageImagePoints& imagePoints, StereoCameraIntrinsics& intrinsics, StereoCameraExtrinsics& extrinsics, const cv::Size& imageSize, const int& flags)
{
    // Calibrate Left Camera
    cv::calibrateCamera(objectPoints.objectPointsLeft,
                        imagePoints.imagePointsLeft,
                        imageSize, intrinsics.cameraMatrixLeft,
                        intrinsics.distortionCoefficientsLeft,
                        extrinsics.rotationVectorsLeft,
                        extrinsics.translationVectorsLeft,
                        flags);

    intrinsics.newCameraMatrixLeft = cv::getOptimalNewCameraMatrix(intrinsics.cameraMatrixLeft,
                                                                    intrinsics.distortionCoefficientsLeft,
                                                                    imageSize,
                                                                    1,
                                                                    imageSize, 
                                                                    0);

    // Calibrate Right Camera
    cv::calibrateCamera(objectPoints.objectPointsRight,
                        imagePoints.imagePointsRight,
                        imageSize,
                        intrinsics.cameraMatrixRight,
                        intrinsics.distortionCoefficientsRight,
                        extrinsics.rotationVectorsRight,
                        extrinsics.translationVectorsRight,
                        flags);
                        
    intrinsics.newCameraMatrixRight = cv::getOptimalNewCameraMatrix(intrinsics.cameraMatrixRight,
                                                                    intrinsics.distortionCoefficientsRight,
                                                                    imageSize,
                                                                    0,
                                                                    imageSize,
                                                                    0);

    int stereoFlags = 0;
    stereoFlags |= cv::CALIB_FIX_INTRINSIC;
    cv::TermCriteria stereoCriteria = cv::TermCriteria(cv::TermCriteria::EPS+cv::TermCriteria::MAX_ITER, 30, 1e-6);

    cv::stereoCalibrate(objectPoints.objectPointsLeft,
                        imagePoints.imagePointsLeft,
                        imagePoints.imagePointsRight,
                        intrinsics.newCameraMatrixLeft,
                        intrinsics.distortionCoefficientsLeft,
                        intrinsics.newCameraMatrixRight,
                        intrinsics.distortionCoefficientsRight,
                        imageSize,
                        extrinsics.rotationVectors,
                        extrinsics.translationVectors,
                        extrinsics.essentialMatrix,
                        extrinsics.fundamentalMatrix,
                        stereoFlags,
                        stereoCriteria);

    return 0;
}

void CalibrationAssistant::computeLensInterpolation(const StereoCameraIntrinsics& intrinstics, const cv::Size& imageSize, StereoCameraLensInterpolation& interpolation)
{
    cv::initUndistortRectifyMap(intrinstics.newCameraMatrixLeft, intrinstics.distortionCoefficientsLeft, cv::Matx33f::eye(), intrinstics.newCameraMatrixLeft, imageSize, CV_32FC1, interpolation.mapXLeft, interpolation.mapYLeft);
    
    cv::initUndistortRectifyMap(intrinstics.newCameraMatrixRight, intrinstics.distortionCoefficientsRight, cv::Matx33f::eye(), intrinstics.newCameraMatrixRight, imageSize, CV_32FC1, interpolation.mapXRight, interpolation.mapYRight);
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

    // Define the objectPoints per image
    std::vector<cv::Point3f> objp;
    for (int i = 0; i < config.boardHeight; i++)
    {
        for (int a = 0; a < config.boardWidth; a++)
        {
            objp.push_back(cv::Point3f(a * config.squareSize, i * config.squareSize, 0));
        }
    }

    // Go through every frame and grab the object and image points
    for (int i = 0; i < config.numberOfFrames; i++)
    {
        StereoImage& image = images.at(i);
        StereoImageChessboardData data;

        findChessboardCorners(image, data, config);
        
        if (data.foundLeft)
        {
            imagePoints.imagePointsLeft.push_back(data.cornersLeft);
            objectPoints.objectPointsLeft.push_back(objp);

            cv::drawChessboardCorners(image.image1, {config.boardHeight, config.boardWidth}, cv::Mat(data.cornersLeft), data.foundLeft);
        }

        if (data.foundRight)
        {
            imagePoints.imagePointsRight.push_back(data.cornersRight);
            objectPoints.objectPointsRight.push_back(objp);

            cv::drawChessboardCorners(image.image2, {config.boardHeight, config.boardWidth}, cv::Mat(data.cornersRight), data.foundRight);
        }
    }

    // calibrating the camera
    StereoCameraIntrinsics intrinsics;
    StereoCameraExtrinsics extrinsics;
    //int flags = cv::CALIB_ZERO_TANGENT_DIST + cv::CALIB_FIX_ASPECT_RATIO + cv::CALIB_FIX_PRINCIPAL_POINT;
    int flags = cv::CALIB_FIX_ASPECT_RATIO + cv::CALIB_FIX_K3 + cv::CALIB_ZERO_TANGENT_DIST + cv::CALIB_FIX_PRINCIPAL_POINT;

    calibrateCamera(objectPoints, imagePoints, intrinsics, extrinsics, images.at(0).image1.size(), flags);

    // computing lens distortion
    StereoCameraLensInterpolation interpolation;
    //computeLensInterpolation(intrinsics, images.at(0).image1.size(), interpolation);

    //cv::fisheye::estimateNewCameraMatrixForUndistortRectify(intrinsics.cameraMatrixLeft, intrinsics.cameraMatrixRight, images.at(0).image1.size(), cv::Matx33d::eye(), newCameraMatrix, 1);
    StereoCameraRectification rect;

    cv::stereoRectify(intrinsics.newCameraMatrixLeft,
                      intrinsics.distortionCoefficientsLeft,
                      intrinsics.newCameraMatrixRight,
                      intrinsics.distortionCoefficientsRight,
                      images.at(0).image1.size(),
                      extrinsics.rotationVectors,
                      extrinsics.translationVectors,
                      rect.rectLeft,
                      rect.rectRight,
                      rect.projMatrixLeft,
                      rect.projMatrixRight,
                      rect.Q,
                      1);

    std::cout << rect.projMatrixLeft << std::endl;
    std::cout << rect.projMatrixRight << std::endl;
    
    cv::initUndistortRectifyMap(intrinsics.cameraMatrixLeft,
                                intrinsics.distortionCoefficientsLeft,
                                rect.rectLeft,
                                //cv::Matx33f::eye(),
                                //rect.projMatrixLeft,
                                intrinsics.newCameraMatrixLeft,
                                images.at(0).image1.size(),
                                CV_32FC1,
                                interpolation.mapXLeft,
                                interpolation.mapYLeft);

    cv::initUndistortRectifyMap(intrinsics.cameraMatrixRight,
                                intrinsics.distortionCoefficientsRight,
                                rect.rectRight,
                                //cv::Matx33f::eye(),
                                //rect.projMatrixRight,
                                intrinsics.newCameraMatrixRight,
                                images.at(0).image1.size(),
                                CV_32FC1,
                                interpolation.mapXRight,
                                interpolation.mapYRight);

    std::cout << intrinsics.newCameraMatrixLeft << std::endl;
    std::cout << intrinsics.newCameraMatrixRight << std::endl;

    for (waveshare::StereoImage image : images)
    {
        cv::Mat undistorted;
        cv::Mat undistorted2;
        cv::Mat img = image.image1;
        cv::Mat img2 = image.image2;

        //cv::undistort(img, undistorted, intrinsics.newCameraMatrixLeft, intrinsics.distortionCoefficientsLeft);
        //cv::undistort(img2, undistorted2, intrinsics.newCameraMatrixRight, intrinsics.distortionCoefficientsRight);

        cv::remap(img,
                  undistorted,
                  interpolation.mapXLeft,
                  interpolation.mapYLeft,
                  //cv::INTER_LANCZOS4,
                  cv::INTER_LINEAR,
                  cv::BORDER_TRANSPARENT,
                  0);

        cv::remap(img2,
                  undistorted2,
                  interpolation.mapXRight,
                  interpolation.mapYRight,
                  //cv::INTER_LANCZOS4,
                  cv::INTER_LINEAR,
                  cv::BORDER_TRANSPARENT,
                  0);

        cv::imshow("Rectified 1", undistorted);
        cv::imshow("Rectified 2", undistorted2);

        cv::waitKey(0);
    }
}