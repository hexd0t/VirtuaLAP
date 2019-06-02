#pragma once

#include "PipelineInterfaces.h"

struct BSplineSample {
    float u;
    glm::vec3 Location;
    glm::vec3 Tangent;
};

class BSpline {
private:

public:
    std::vector<BSplineSample> Discretize(TrackGeometry *track);
    std::vector<glm::vec3> Extrude(const std::vector<BSplineSample>& samples);

private:
    float bsplineBase(int i, int p, float u, const std::vector<float>& knots) const;
    glm::vec3 bsplinePoint(float u, int degree, const std::vector<float>& knots, TrackGeometry* cps) const;
    glm::vec3 bsplineTangent(float u, int degree, const std::vector<float>& knots, TrackGeometry* cps) const;
    void addSamples(const BSplineSample& currentSample, const BSplineSample& nextSample,
                    std::vector<BSplineSample> &points,  //Outputs
                    const std::vector<float> &knots, TrackGeometry *cps, float maxTangentChange) const; //Constant inputs
};