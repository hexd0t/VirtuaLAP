#include <iostream>
#include <iomanip>
#include "Render_BSpline.h"

constexpr unsigned int TRACKDEGREE = 3;
const float MAXATANGENTANGLE = cosf(10.f / 180.f * 3.141f);
constexpr float epsilon = 0.0001f;
constexpr float MAXUDELTA = 0.25f;

std::vector<glm::vec3> BSpline::Triangulate(TrackGeometry *track) {
    if(track->ControlPoints.size()<2)
        return std::vector<glm::vec3>();
    unsigned int cpcount = track->ControlPoints.size();
    unsigned int knotcount = cpcount + TRACKDEGREE + 1;
    std::vector<float> knots(knotcount, 0.f);

    int repcount = 0;
    float curknot = 1.f;
    for (int i = TRACKDEGREE+1; i < knotcount-1; ++i) {
        knots[i] = curknot;
        ++repcount;
        if(repcount == TRACKDEGREE) {
            repcount = 0;
            curknot += 1.f;
        }
    }
    knots[knotcount-1] = knots[knotcount-2];//Last knot hat to be repeated degree + 1 times

    /*for(auto &k : knots) {
        std::cout << k << " ";
    }
    std::cout << std::endl;*/

    float prevU = 0.f;
    glm::vec3 prevTangent = bsplineTangent(prevU, TRACKDEGREE, knots, track);
    std::vector<std::pair<float, glm::vec3>> points; //calculate u, position for spline
    points.emplace_back(std::pair<float, glm::vec3>(prevU, bsplinePoint(prevU, TRACKDEGREE, knots, track)));

    float lastU = knots[knotcount-1] + epsilon;

    for(float u = prevU + MAXUDELTA; u <= lastU; ) {
        glm::vec3 tangent = bsplineTangent(u, TRACKDEGREE, knots, track);

        //std::cout << std::setw(8) << tangent.x << "," << std::setw(8) << tangent.y << "," << std::setw(8) << tangent.z << std::endl;
        evaluateUntilTangent(prevU, prevTangent, u, tangent, points,
                knots, track, MAXATANGENTANGLE);/**/
        points.emplace_back(std::pair<float, glm::vec3>(u, bsplinePoint(u, TRACKDEGREE, knots, track)));

        std:: cout << std::endl;
        prevTangent = tangent;
        prevU = u;
        u += MAXUDELTA;
    }

    std::vector<glm::vec3> result;
    for(auto &it:points)
        result.push_back(it.second);

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

glm::vec3 BSpline::bsplinePoint(float u, int degree, const std::vector<float> &knots, TrackGeometry *cps) const {
    glm::vec3 result(0.f, 0.f, 0.f);
    auto& points = cps->ControlPoints;
    //std::cout << u << ":";
    for(int i = 0; i < points.size(); ++i) {//ToDo: only calculate contributing weights
        result += bsplineBase(i, degree, u, knots) * points[i].Location;
    }
    //std::cout<<std::endl;
    return result;
}

glm::vec3 BSpline::bsplineTangent(float u, int degree, const std::vector<float> &knots, TrackGeometry *cps) const {
    glm::vec3 result(0.f, 0.f, 0.f);
    auto& points = cps->ControlPoints;
    if(u >= *knots.rbegin()) //Fix numerical instability with last point not evaluating correctly
        u-= epsilon;

    for(int i = 0; i < points.size()-1; ++i) {//ToDo: only calculate contributing weights
        float qdenom = (knots[i+degree+1] - knots[i+1]);

        result += bsplineBase(i+1, degree-1, u, knots) *
                (degree / qdenom) *
                (points[i+1].Location - points[i].Location);
    }
    return glm::normalize(result);
}

void BSpline::evaluateUntilTangent(float currentU, const glm::vec3 &currentTangent, float nextU, const glm::vec3 &nextTangent,
                                   std::vector<std::pair<float, glm::vec3>> &points,
                                   const std::vector<float> &knots, TrackGeometry *cps, float maxTangentChange) const {
    float cosangle = glm::dot(currentTangent, nextTangent);

    /*std::cout << std::setprecision(4) << std::setw(8) << currentU << " " << std::setw(8) << nextU
        << " @ " << std::setw(8)<< acosf(cosangle)*180.f/3.141f << "° | ";/**/
        //<< std::setw(8) << currentTangent.x << "," << std::setw(8) << currentTangent.y << "," << std::setw(8) << currentTangent.z << " "
        //<< std::setw(8) << nextTangent.x << "," << std::setw(8) << nextTangent.y << "," << std::setw(8) << nextTangent.z;
    float udiff = std::abs(currentU-nextU);
    if(udiff < 0.002) {
        std::cerr << "Spline cannot reach requested LOD!" << std::endl;
        return;
    }

    if(cosangle < maxTangentChange) { //since we compare cos results, a smaller angle will have a bigger value
        //std::cout << std::endl;

        float middleU = (nextU - currentU) * 0.5f + currentU;
        glm::vec3 middleTangent = bsplineTangent(middleU, TRACKDEGREE, knots, cps);
        evaluateUntilTangent(currentU, currentTangent, middleU, middleTangent,
                points, knots, cps, maxTangentChange);
        points.emplace_back(std::pair<float, glm::vec3>(middleU, bsplinePoint(middleU, TRACKDEGREE, knots, cps)));
        evaluateUntilTangent(middleU, middleTangent, nextU, nextTangent,
                points, knots, cps, maxTangentChange);
    }
    /*else
    {
        std::cout << ";" << std::endl;
    }*/
}
