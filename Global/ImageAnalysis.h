#pragma once

#include "PipelineInterfaces.h"
#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <chrono>


using namespace std::chrono;

class ImageAnalysis {
private: //Class members
    ImageAnalysisState _state;
    time_point<system_clock> _calibLastFrameTime;
    std::vector<std::vector<int>> _calibFramesIds;
    std::vector<std::vector<cv::Point2f>> _calibFramesCorners;
    double _calibCurrentError = -1.0;

    cv::Mat _camera;
    cv::Mat _distortion;
    cv::Ptr<cv::aruco::Dictionary> _dict;
    cv::Ptr<cv::aruco::CharucoBoard> _calibBoard;
public:  //Public methods
    ImageAnalysis();
    ~ImageAnalysis();

    void Step(const CameraImageData* cameraImage,ImageAnalysisResult* result);
    void ChangeState(const ImageAnalysisState& newstate);
private: //Private methods
    void _calibrate(cv::Mat& cameraImage);
};

