#pragma once

#include "PipelineInterfaces.h"

class Simulation {
private:
public:
    Simulation();
    ~Simulation();

    void Step(ImageAnalysisResult* imgAnalysis, TrackGeometry* track, float deltaT, GameState* output); //Calculate a single frame update

};
