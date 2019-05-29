//
// Created by Unknown on 05.05.2019.
//

#include "ImageAnalysis.h"
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <iostream>
#include "glm/glm/gtc/matrix_transform.hpp"
//#include <opencv2/highgui.hpp> //Enable for debugging using cv::imshow, also requires adding highgui in Desktop/CMakeList.txt

using namespace std::chrono;

ImageAnalysis::ImageAnalysis() :
    _state((ImageAnalysisState)(ImageAnalysis_Calibrating | ImageAnalysis_DebugOverlay)) {

}

ImageAnalysis::~ImageAnalysis() {

}

void ImageAnalysis::Step(const CameraImageData *cameraImage, ImageAnalysisResult *result) {
    //ToDo: Implement image Analysis here. For debugging purposes, you can edit the camera image to draw stuff to the output window
    //Also, contents of the result struct will be printed
    cv::Mat inputImage(cameraImage->Height, cameraImage->Width, CV_8UC3, cameraImage->Data);

    switch(_state & 0xFF) {
        case ImageAnalysis_Calibrating: {
            //Do camera calibration
            //Maybe use a "ChArUco" marker as the start/finish marker?
            //Always specify distances in millimeters

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
                    glm::vec3(300,0,0),
                    glm::vec3(0,-1,0),
                    glm::vec3(0,0,1)
            });
            break;
        }
        case ImageAnalysis_Unknown:
        default:
            std::cerr << "ImgAnalysis state: "<< std::hex << _state << std::endl;
            throw std::logic_error("Image analysis got into unknown state");
    }

    //Test drawing onto the image
    if(_state & ImageAnalysis_DebugOverlay) //Press F2 to toggle
        cv::rectangle(inputImage, cv::Point(200.f,200.f), cv::Point(240.f, 240.f), cv::Scalar(0, 255, 0), 2);

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
        default:
            throw std::logic_error("State switch not implemented");
    }
}
