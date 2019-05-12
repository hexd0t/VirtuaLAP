#pragma once

#include "PipelineInterfaces.h"

class TrackGeneration {
private: //Class members

public:  //Public methods
    TrackGeneration();
    ~TrackGeneration();

    void Step(ImageAnalysisResult* input, TrackGeometry* output);
private: //Private methods

};
