#include <iostream>
#include "Render_BSpline.h"

constexpr unsigned int degree = 3;
constexpr float epsilon = 0.0001f;

std::vector<glm::vec3> BSpline::Triangulate(TrackGeometry *track) {
    if(track->ControlPoints.size()<2)
        return std::vector<glm::vec3>();
    unsigned int cpcount = track->ControlPoints.size();
    unsigned int knotcount = cpcount + degree + 1;
    std::vector<float> knots(knotcount, 0.f);

    int repcount = 0;
    float curknot = 1.f;
    for (int i = degree+1; i < knotcount-1; ++i) {
        knots[i] = curknot;
        ++repcount;
        if(repcount == degree) {
            repcount = 0;
            curknot += 1.f;
        }
    }
    knots[knotcount-1] = knots[knotcount-2];//Last knot hat to be repeated degree + 1 times

    std::vector<glm::vec3> result;
    for(float u = 0.f; u < knots[knotcount-1]; u+=0.1f) {
        result.push_back(bsplinePoint(u, knots, track));
    }

    return result;
}

float BSpline::bsplineBase(int i, int p, float u, const std::vector<float>& knots) const {
    if(p == 0)
    {
        float result = (knots[i] <= u && u < knots[i+1]) ? 1.f : 0.f;
        //std::cout << result;
        return result;
    }

    float denomA = knots[i+p] - knots[i];
    float denomB = knots[i+p+1] - knots[i+1];
    float coeffA = denomA > epsilon ? (u - knots[i]) / denomA : 0.f;
    float coeffB = denomB > epsilon ? (knots[i+p+1] - u) / denomB : 0.f;


    //std::cout << "(";
    float baseA = bsplineBase(i, p-1, u, knots);
    //std::cout << " | ";
    float baseB = bsplineBase(i+1, p-1, u, knots);
    //std::cout << ") ";
    float result = coeffA * baseA + coeffB * baseB;
    //std::cout << result;

    return result;
}

glm::vec3 BSpline::bsplinePoint(float u, const std::vector<float> &knots, TrackGeometry *cps) const {
    glm::vec3 result(0.f, 0.f, 0.f);
    auto& points = cps->ControlPoints;
    //std::cout << u << ":";
    for(int i = 0; i < points.size(); ++i) {//ToDo: only calculate contributing weights
        result += bsplineBase(i, degree, u, knots) * points[i].Location;
    }
    //std::cout<<std::endl;
    return result;
}
