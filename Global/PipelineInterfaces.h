#pragma once

#include <vector>
#include "glm/glm/glm.hpp" //Math Types

struct CameraImageData {
    int Width;
    int Height;
    char* Data; //RGB data stream
};

enum ImageAnalysisState {
    ImageAnalysis_Unknown       = 0x0000,
    ImageAnalysis_Calibrating   = 0x0001,
    ImageAnalysis_Operating     = 0x0002,

    ImageAnalysis_Simulating    = 0x0012
            ,
    ImageAnalysis_DebugOverlay  = 0x1000
};
struct MarkerInfo {
    int id;
    glm::vec3 Location;
    glm::vec3 Direction;
    glm::vec3 Normal;
};
struct ImageAnalysisResult {
    ImageAnalysisState State;
    glm::vec3 CameraLocation;
    glm::vec3 CameraLookDirection;
    glm::vec3 CameraUp;
    std::vector<MarkerInfo> Markers;
};

struct TrackControlPoint {
    glm::vec3 Location;
    glm::vec3 Normal;
    //ToDo(AMü): Additional info, like pavement type
};
struct TrackGeometry {
    //ToDo(AMü): Should the generator already triangulate this?
    std::vector<TrackControlPoint> ControlPoints;
};

struct GameState {

};