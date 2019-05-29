#pragma once

#include "PipelineInterfaces.h"

class BSpline {
private:

public:
    std::vector<glm::vec3> Triangulate(TrackGeometry* track);

private:
    float bsplineBase(int i, int p, float u, const std::vector<float>& knots) const;
    glm::vec3 bsplinePoint(float u, const std::vector<float>& knots, TrackGeometry* cps) const;
};