#pragma once

#include "ImageAnalysis.h"
#include "TrackGeneration.h"
#include "Simulation.h"

#include "PlatformInterfaces.h"

/*********************************************************\
| This is the interface toward the platform specific code |
\*********************************************************/
class Core {
private: //Class members
    //Subsystems:
    ImageAnalysis _analysis;
    TrackGeneration _generator;
    Simulation _simulation;

    //Platform callbacks:
    CaptureImageFunc captureImage;

public:  //Public methods
    Core(CaptureImageFunc);
    ~Core();

    void StartPipeline();
    void Step();
private: //Private methods

};
