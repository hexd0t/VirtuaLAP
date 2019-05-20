#pragma once

#include "ImageAnalysis.h"
#include "TrackGeneration.h"
#include "Simulation.h"
#include "Render.h"

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
    Render _render;

    //Platform callbacks:
    CaptureImageFunc captureImage;

public:  //Public methods
    Core(CaptureImageFunc);
    ~Core();
    void Init();

    void StartPipeline();
    void Step(float deltaT);

    void FramebufferSizeChanged(int width, int height);
private: //Private methods

};
