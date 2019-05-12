#pragma once

#include "PipelineInterfaces.h"

class Simulation {
private: //Class members

public:  //Public methods
    Simulation();
    ~Simulation();
    void Step(CameraImageData* camImage, ImageAnalysisResult* imgAnalysis, TrackGeometry* track); //Calculate a single frame and render it
private: //Private methods

};
