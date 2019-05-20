#include <utility>

//
// Created by Unknown on 05.05.2019.
//

#include "Core.h"

Core::Core(CaptureImageFunc capImage)
    : _analysis(), _generator(), _simulation(),  _render(),
    captureImage(std::move(capImage))
{

}

Core::~Core() {

}

void Core::StartPipeline() {
    //ToDo(AMü): Run Camera capture, image analysis and track generation in seperate threads,
    //Synced by single-producer, single-consumer queues
}

void Core::Step(float deltaT) {
    CameraImageData camImg = {};
    ImageAnalysisResult analysisResult = {};
    TrackGeometry track = {};
    GameState gameState =  {};

    captureImage(&camImg);
    _analysis.Step(&camImg, &analysisResult);
    if (analysisResult.State == ImageAnalysis_Operating){ //ToDo: Add simulation state (scanning track, simulating etc)
        _generator.Step(&analysisResult, &track);
        _simulation.Step(&analysisResult, &track, deltaT, &gameState);
    }
    _render.Step(&camImg, &analysisResult, &track, deltaT, &gameState);
}

void Core::FramebufferSizeChanged(int width, int height) {
    _render.FramebufferSizeChanged(width, height);
}

void Core::Init() {
    _render.Init();
}

