#pragma once

#include <vector>
#include "glm/glm/glm.hpp" //Math Types

struct CameraImageData {
    int Width;
    int Height;
    char* Data; //RGB data stream
};

enum ImageAnalysisState {
    ImageAnalysis_Unknown = 0,
    ImageAnalysis_Calibrating = 1,
    ImageAnalysis_Operating = 2
};
struct ImageAnalysisResult {
    ImageAnalysisState State;
    glm::vec3 CameraLocation;
    std::vector<std::pair<int, glm::vec3>> MarkerLocations;
};

struct TrackControlPoint {
    glm::vec3 Location;
    glm::vec3 Direction;
    glm::vec3 Normal;
    //ToDo(AMü): Additional info, like pavement type
};
struct TrackGeometry {
    //ToDo(AMü): Should the generator already triangulate this?
    std::vector<TrackControlPoint> ControlPoints;
};

struct GameState {

};