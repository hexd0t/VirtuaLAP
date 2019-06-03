//
// Created by Unknown on 05.05.2019.
//

#include "ImageAnalysis.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "glm/glm/gtc/matrix_transform.hpp"
#include <opencv2/highgui.hpp> //Enable for debugging using cv::imshow, also requires adding highgui in Desktop/CMakeList.txt
#include <opencv2/calib3d.hpp>

constexpr int CALIBRATIONFRAMESPACING = 1000; //ms

ImageAnalysis::ImageAnalysis() :
    _state((ImageAnalysisState)(ImageAnalysis_Calibrating | ImageAnalysis_DebugOverlay)),
    _dict(cv::aruco::Dictionary::get(cv::aruco::DICT_5X5_250)),
    _calibBoard(cv::aruco::CharucoBoard::create(7, 7, 30, 20, _dict))
{
    _detectorParams = cv::aruco::DetectorParameters::create();
    //Note: If we're having performance problems, trying other refinements is an easy optimization:
    _detectorParams->cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
}

ImageAnalysis::~ImageAnalysis() {

}

void ImageAnalysis::Step(const CameraImageData *cameraImage, ImageAnalysisResult *result) {
    //ToDo: Implement image Analysis here. For debugging purposes, you can edit the camera image to draw stuff to the output window
    //Also, contents of the result struct will be printed
    cv::Mat inputImage(cameraImage->Height, cameraImage->Width, CV_8UC3, cameraImage->Data);
    result->TempCameraMat = glm::mat4(1.f);

    switch(_state & 0xFF) {
        case ImageAnalysis_Calibrating: {
            //Always specify distances in millimeters
            time_point<system_clock> now = system_clock::now();
            auto calibAge = duration_cast<milliseconds>(now - _calibLastFrameTime);
            //if(calibAge.count() >= CALIBRATIONFRAMESPACING) {
                _calibrate(inputImage);
                _calibLastFrameTime = now;
            //}
            break;
        }
        case ImageAnalysis_Operating: {
            _detectMarkers(inputImage, result);
            break;
        }
        case ImageAnalysis_Simulating: {
            float clock_ms = duration_cast<milliseconds>(
                    time_point_cast<milliseconds>(std::chrono::system_clock::now()).time_since_epoch() ).count() % (3141 * 2);

            auto rotate = glm::rotate(glm::mat4(1.0f), clock_ms*0.001f, glm::vec3(0,0,1));
            result->CameraLocation = rotate * glm::vec4(1000, 0, 1000, 1);
            result->CameraLookDirection = glm::normalize(-result->CameraLocation);
            result->CameraUp = glm::vec3(0, 0, 1);

            result->Markers.emplace_back(MarkerInfo {
                    0, //Marker 0 always should have these properties, since it defines the origin
                    glm::vec3(0,0,0),
                    glm::vec3(0,1,0),
                    glm::vec3(0,0,1)
            });
            result->Markers.emplace_back(MarkerInfo {
                    1,
                    glm::vec3(600,0,0),
                    glm::vec3(0,-1,0),
                    glm::vec3(0,0,1)
            });
            result->Markers.emplace_back(MarkerInfo {
                    2,
                    glm::vec3(250,-400,100),
                    glm::vec3(0,1,0),
                    glm::vec3(0,0,1)
            });
            result->Markers.emplace_back(MarkerInfo {
                    3,
                    glm::vec3(400,0,30),
                    glm::vec3(-0.5,0.5,0),
                    glm::vec3(0,0,1)
            });
            break;
        }
        case ImageAnalysis_MarkerOutput: {
            cv::Mat marker(1000, 1000, CV_8UC3);
            _calibBoard->draw(marker.size(), marker);
            cv::imshow("board", marker);
            cv::waitKey(0);
            break;
        }
        case ImageAnalysis_Unknown:
        default:
            std::cerr << "ImgAnalysis state: "<< std::hex << _state << std::endl;
            throw std::logic_error("Image analysis got into unknown state");
    }
    result->CalibrationError = _calibCurrentError;
    result->State = _state;
}

void ImageAnalysis::ChangeState(const ImageAnalysisState &newstate) {
    switch (newstate) {
        case ImageAnalysis_Simulating:
            if((_state & 0xFF) != ImageAnalysis_Simulating) {
                //For switching to simulating, we don't need to be in a specific prior state
                _state = ImageAnalysis_Simulating;
            }
            else
                _state = ImageAnalysis_Calibrating;
            break;
        case ImageAnalysis_DebugOverlay:
            _state = (ImageAnalysisState)(_state ^ ImageAnalysis_DebugOverlay); //Toggle Overlay
            break;
        case ImageAnalysis_MarkerOutput:
            _state = ImageAnalysis_MarkerOutput;
            break;
        default:
            throw std::logic_error("State switch not implemented");
    }
}

void ImageAnalysis::_calibrate(cv::Mat& cameraImage) {

    std::vector< int > markerIds;
    std::vector< std::vector< cv::Point2f > > markerCorners, markerRejected;
    cv::aruco::detectMarkers(cameraImage, _dict, markerCorners, markerIds,
                             _detectorParams, markerRejected);

    if(markerIds.empty())
        return;

    if(_state & ImageAnalysis_DebugOverlay) {
        cv::aruco::drawDetectedMarkers(cameraImage, markerCorners, markerIds, cv::Scalar(0,255,0));
        cv::aruco::drawDetectedMarkers(cameraImage, markerRejected, cv::noArray(), cv::Scalar(255,0,0));
    }

    std::vector< cv::Point2f > charucoCorners;
    std::vector< int > charucoIds;
    cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, cameraImage,
            _calibBoard, charucoCorners, charucoIds);

    if(charucoIds.size() < 2)
        return;

    if(_state & ImageAnalysis_DebugOverlay) {
        cv::aruco::drawDetectedCornersCharuco(cameraImage, charucoCorners, charucoIds, cv::Scalar(0,0,255));
    }

    _calibFramesCorners.push_back(charucoCorners);
    _calibFramesIds.push_back(charucoIds);

    _calibCurrentError = cv::aruco::calibrateCameraCharuco(_calibFramesCorners,
            _calibFramesIds, _calibBoard, cameraImage.size(), _camera, _distortion);

    if(_calibFramesIds.size() >= 20 && _calibCurrentError < 2.0) {
        _state = static_cast<ImageAnalysisState>(
                (_state | ImageAnalysis_Operating) & ~ImageAnalysis_Calibrating);

        std::cout << "Cam:" << std::endl;
        for(size_t j = 0; j < 3; ++j) {
            for (size_t k = 0; k < 3; ++k) {
                std::cout << _camera.at<float>(j, k) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Dist:" << std::endl;
        for(size_t j = 0; j < 4; ++j) {
            std::cout << _distortion.at<float>(j) << " ";
            std::cout << std::endl;
        }
    }
}

void ImageAnalysis::_detectMarkers(cv::Mat &cameraImage, ImageAnalysisResult* result) {
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f> > markerCorners;
    cv::aruco::DetectorParameters detectorParams;

    cv::aruco::detectMarkers(cameraImage, _dict, markerCorners, markerIds,
                             _detectorParams, cv::noArray(), _camera, _distortion);

    if(markerIds.empty())
        return;

    if(_state & ImageAnalysis_DebugOverlay) {
        cv::aruco::drawDetectedMarkers(cameraImage, markerCorners, markerIds, cv::Scalar(0,128,0));
    }

    std::vector<cv::Vec3d> rvecs, tvecs;
    cv::aruco::estimatePoseSingleMarkers(markerCorners, 30.f, _camera, _distortion, rvecs, tvecs);

    for(size_t i = 0; i < markerIds.size(); ++i){
        cv::aruco::drawAxis(cameraImage, _camera, _distortion, rvecs[i], tvecs[i], 40.f);
        if(markerIds[i] == 3) {
            result->CameraLocation = glm::vec3(
                    tvecs[i][0], -tvecs[i][1], tvecs[i][2]
                    );

            cv::Mat rotation;

            float theta = cv::norm(rvecs[i]);
            glm::vec3 rotateAxis(rvecs[i][0]/theta, rvecs[i][1]/theta, rvecs[i][2]/theta);
            //std::cout << theta << " " << rotateAxis.x << " " << rotateAxis.y << " " << rotateAxis.z << std::endl;
            glm::mat4 glmRotate = glm::rotate(glm::mat4(1), theta, rotateAxis);

            /*for(size_t j = 0; j < 4; ++j) {
                for (size_t k = 0; k < 4; ++k) {
                    std::cout << glmRotate[j][k] << " ";
                }
                std::cout << std::endl;
            }*/


            /*cv::Rodrigues(rvecs[i], rotation);
            result->TempCameraMat = glm::mat4(1.f);
            for(size_t j = 0; j < 3; ++j) {
                for (size_t k = 0; k < 3; ++k) {
                    result->TempCameraMat[j][k] = rotation.at<float>(j, k);
                    std::cout << rotation.at<float>(j, k) << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;*/
            result->TempCameraMat = glm::translate(glmRotate, result->CameraLocation);
        }
    }
}
