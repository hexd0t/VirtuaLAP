//
// Created by Unknown on 05.05.2019.
//

#include "ImageAnalysis.h"
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>


ImageAnalysis::ImageAnalysis() : _state(ImageAnalysis_Calibrating) {

}

ImageAnalysis::~ImageAnalysis() {

}

void ImageAnalysis::Step(const CameraImageData *cameraImage, ImageAnalysisResult *result) {
    //ToDo: Implement image Analysis here. For debugging purposes, you can edit the camera image to draw stuff to the output window
    //Also, contents of the result struct will be printed

    switch(_state) {
        case ImageAnalysis_Calibrating: {
            //Do camera calibration
            break;
        }
        case ImageAnalysis_Operating: {
            //Example code from docs:
            cv::Mat inputImage(cameraImage->Width, cameraImage->Height, CV_8UC3, cameraImage->Data);
            std::vector<int> markerIds;
            std::vector<std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
            cv::aruco::DetectorParameters parameters;
            cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
            //ToDo: Provide Parameters etc
            //cv::aruco::detectMarkers(inputImage, &dictionary, markerCorners, markerIds, &parameters, rejectedCandidates);
            break;
        }
        case ImageAnalysis_Unknown:
        default:
            throw std::logic_error("Image analysis got into unknown state");
    }
    result->State = _state; //Communicate current state to other modules
}
