#pragma once

#include "PipelineInterfaces.h"

class BSpline {
private:

public:
    std::vector<glm::vec3> Triangulate(TrackGeometry* track);

private:
    float bsplineBase(int i, int p, float u, const std::vector<float>& knots) const;
    glm::vec3 bsplinePoint(float u, int degree, const std::vector<float>& knots, TrackGeometry* cps) const;
    glm::vec3 bsplineTangent(float u, int degree, const std::vector<float>& knots, TrackGeometry* cps) const;
    void evaluateUntilTangent(float currentU, const glm::vec3& currentTangent, float nextU, const glm::vec3& nextTangent, //Recursive inputs
            std::vector<std::pair<float, glm::vec3>>& points,  //Outputs
            const std::vector<float>& knots, TrackGeometry* cps, float maxTangentChange) const; //Constant inputs
};