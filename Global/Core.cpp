#include <utility>

//
// Created by Unknown on 05.05.2019.
//

#include "Core.h"

Core::Core(CaptureImageFunc capImage)
    : _analysis(), _generator(), _simulation(),
    captureImage(std::move(capImage))
{

}

Core::~Core() {

}

void Core::StartPipeline() {
    //ToDo(AMü): Run Camera capture, image analysis and track generation in seperate threads,
    //Synced by single-producer, single-consumer queues
}

void Core::Step() {
    CameraImageData camImg;
    ImageAnalysisResult analysisResult;
    TrackGeometry track;

    captureImage(&camImg);
    _analysis.Step(&camImg, &analysisResult);
    if (analysisResult.State == ImageAnalysis_Operating){ //ToDo: Add simulation state (scanning track, simulatin etc)
        _generator.Step(&analysisResult, &track);
    }
    _simulation.Step(&camImg, &analysisResult, &track);
}

