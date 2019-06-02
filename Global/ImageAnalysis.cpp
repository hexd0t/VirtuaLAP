//
// Created by Unknown on 05.05.2019.
//

#include "ImageAnalysis.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "glm/glm/gtc/matrix_transform.hpp"
#include <opencv2/highgui.hpp> //Enable for debugging using cv::imshow, also requires adding highgui in Desktop/CMakeList.txt

constexpr int CALIBRATIONFRAMESPACING = 1000; //ms

ImageAnalysis::ImageAnalysis() :
    _state((ImageAnalysisState)(ImageAnalysis_Calibrating | ImageAnalysis_DebugOverlay)),
    _dict(cv::aruco::Dictionary::get(cv::aruco::DICT_5X5_250)),
    _calibBoard(cv::aruco::CharucoBoard::create(5, 5, 30, 20, _dict))
{

}

ImageAnalysis::~ImageAnalysis() {

}

void ImageAnalysis::Step(const CameraImageData *cameraImage, ImageAnalysisResult *result) {
    //ToDo: Implement image Analysis here. For debugging purposes, you can edit the camera image to draw stuff to the output window
    //Also, contents of the result struct will be printed
    cv::Mat inputImage(cameraImage->Height, cameraImage->Width, CV_8UC3, cameraImage->Data);

    switch(_state & 0xFF) {
        case ImageAnalysis_Calibrating: {
            //Always specify distances in millimeters
            time_point<system_clock> now = system_clock::now();
            auto calibAge = duration_cast<milliseconds>(now - _calibLastFrameTime);
            if(calibAge.count() >= CALIBRATIONFRAMESPACING) {
                _calibrate(inputImage);
                _calibLastFrameTime = now;
            }
            break;
        }
        case ImageAnalysis_Operating: {
            //Example code from docs:

            std::vector<int> markerIds;
            std::vector<std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
            cv::aruco::DetectorParameters parameters;
            cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
            //ToDo: Provide Parameters etc
            //cv::aruco::detectMarkers(inputImage, &dictionary, markerCorners, markerIds, &parameters, rejectedCandidates);
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
            if(_state & 0xFF != ImageAnalysis_Simulating) {
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
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams =
            cv::aruco::DetectorParameters::create();
    detectorParams->cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
    //Note: if we're performance constrained, this would probably work without refinement

    std::vector< int > markerIds;
    std::vector< std::vector< cv::Point2f > > markerCorners, markerRejected;
    cv::aruco::detectMarkers(cameraImage, _dict, markerCorners, markerIds,
            detectorParams, markerRejected);

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

    if(charucoIds.empty())
        return;

    if(_state & ImageAnalysis_DebugOverlay) {
        cv::aruco::drawDetectedCornersCharuco(cameraImage, charucoCorners, charucoIds, cv::Scalar(0,0,255));
    }

    _calibFramesCorners.push_back(charucoCorners);
    _calibFramesIds.push_back(charucoIds);

    _calibCurrentError = cv::aruco::calibrateCameraCharuco(_calibFramesCorners,
            _calibFramesIds, _calibBoard, cameraImage.size(), _camera, _distortion);

    if(_calibFramesIds.size() > 5 && _calibCurrentError < 1.0) {
        _state = ImageAnalysis_Operating;
    }
}
