//
// Created by Unknown on 05.05.2019.
//

#include "ImageAnalysis.h"
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/highgui.hpp> //Enable for debugging using cv::imshow, also requires adding highgui in Desktop/CMakeList.txt


ImageAnalysis::ImageAnalysis() : _state(ImageAnalysis_Calibrating) {

}

ImageAnalysis::~ImageAnalysis() {

}

void ImageAnalysis::Step(const CameraImageData *cameraImage, ImageAnalysisResult *result) {
    //ToDo: Implement image Analysis here. For debugging purposes, you can edit the camera image to draw stuff to the output window
    //Also, contents of the result struct will be printed
    cv::Mat inputImage(cameraImage->Height, cameraImage->Width, CV_8UC3, cameraImage->Data);

    switch(_state) {
        case ImageAnalysis_Calibrating: {
            //Do camera calibration
            //Maybe use a "ChArUco" marker as the start/finish marker?

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
        case ImageAnalysis_Unknown:
        default:
            throw std::logic_error("Image analysis got into unknown state");
    }

    cv::rectangle(inputImage, cv::Point(200.f,200.f), cv::Point(240.f, 240.f), cv::Scalar(0, 255, 0), 2);

    return;
}
