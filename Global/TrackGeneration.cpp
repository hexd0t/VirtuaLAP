//
// Created by Unknown on 05.05.2019.
//

#include "TrackGeneration.h"

constexpr float TANGENTCONTROLDISTANCE = 50.f;

TrackGeneration::TrackGeneration() {

}

TrackGeneration::~TrackGeneration() {

}

void TrackGeneration::Step(ImageAnalysisResult *input, TrackGeometry *output) {
    if(input->Markers.size() < 2)
        return; //Can't build a track without at least 2 markers
    auto first = input->Markers[0];
    output->ControlPoints.emplace_back(TrackControlPoint {
        first.Location, first.Normal
    });
    output->ControlPoints.emplace_back(TrackControlPoint {
            moveAlongDirection(first.Location, first.Direction, TANGENTCONTROLDISTANCE),
            first.Normal
    });
    for (size_t i = 1; i < input->Markers.size(); ++i) {
        auto src = input->Markers[i];
        auto cp = TrackControlPoint {
                src.Location,
                src.Normal
        };
        output->ControlPoints.emplace_back(TrackControlPoint {
                moveAlongDirection(src.Location, src.Direction, -TANGENTCONTROLDISTANCE),
                src.Normal
        });
        output->ControlPoints.push_back(cp);
        output->ControlPoints.push_back(cp); // For 2nd degree spline, push points 2x

        output->ControlPoints.emplace_back(TrackControlPoint {
                moveAlongDirection(src.Location, src.Direction, TANGENTCONTROLDISTANCE),
                src.Normal
        });
    }
    output->ControlPoints.emplace_back(TrackControlPoint {
            moveAlongDirection(first.Location, first.Direction, -TANGENTCONTROLDISTANCE),
            first.Normal
    });
    output->ControlPoints.emplace_back(TrackControlPoint {
            first.Location, first.Normal
    });
}

glm::vec3 TrackGeneration::moveAlongDirection(const glm::vec3& start, const glm::vec3 &dir, float distance) const {
    return start + glm::normalize(dir)*distance;
}
