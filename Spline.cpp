#include "Spline.h"
#include <iostream>
// Implementation of Spline::Bezier, Spline::BSpline, Spline::Subdiv.
// Each of these functions have input
//    ControlCurve* control,
//    Curve* curve,
//   and some int for resolution.
// Each function should take information from control (specifically, the
// list of point positions control->P ) and add points to the 2nd curve.
// When these functions are called, curve->P is an empty vector<vec2>.
// You may use Curve::addPoint( glm::vec2 position ) to append a new point
// position to curve.


typedef std::vector<glm::vec2> vec2s;

// Some possible helper functions

// Given a list of points and t in [0,1], output Bezier point at t
glm::vec2 deCasteljau(vec2s P_in, float t){
    int recursions = P_in.size();
    for (int u = 0; u < recursions; u++) {
        for (int i = 0; i < P_in.size() - u - 1; i++) {
            //Lerp between two points and replace the first point of the pair (Ignoring the last point)
            P_in[i] = glm::mix(P_in[i], P_in[i + 1], t);
        }
    }
    return P_in[0];
}

// Given 4 points and some t in [2,3], output the BSpline point at t
glm::vec2 BSplineHelper(glm::vec2 P1,
                        glm::vec2 P2,
                        glm::vec2 P3,
                        glm::vec2 P4, float t)
{
    vec2s points = vec2s{P1, P2, P3, P4};
    int lerpA;
    int lerpB;

    int recursions = points.size() - 1;
    for (int u = 0; u < recursions; u++) {
        lerpA = u;
        lerpB = 3;
        for (int i = 0; i < points.size() - u - 1; i++) {
            //Barycentric lerp
            points[i] = ((lerpB - t) / (lerpB - lerpA)) * points[i] + ((t - lerpA) / (lerpB - lerpA)) * points[i + 1];
            lerpA += 1;
            lerpB += 1;
        }
    }

    return points[0];
}

// Given n points, find 2*n points that are the result of subdivision.
vec2s SubdivLeft(vec2s P_in) { // first n points
    vec2s newPoints(P_in.size());
    int recursions = P_in.size();

    for (int u = 0; u < recursions; u++) {
        //Before replacing P[0] each recursion, store it as a new subdivided point
        newPoints[u] = P_in[0];
        for (int i = 0; i < P_in.size() - u - 1; i++) {
            //Lerp between two points and replace the first point of the pair (Ignoring the last point)
            P_in[i] = glm::mix(P_in[i], P_in[i + 1], 0.5);
        }
    }
    newPoints[newPoints.size() - 1] = P_in[0]; //The last new point

    return newPoints;
} 
vec2s SubdivRight(vec2s P_in) { // The (n+1)th to 2n-th points.
    //Using de Casteljau will simply give us the correct vector of right side points
    int recursions = P_in.size();
    for (int u = 0; u < recursions; u++) {
        for (int i = 0; i < P_in.size() - u - 1; i++) {
            //Lerp between two points and replace the first point of the pair (Ignoring the last point)
            P_in[i] = glm::mix(P_in[i], P_in[i + 1], 0.5);
        }
    }
    return P_in;
}


void Spline::Bezier( ControlCurve* control, Curve* curve, int resolution ){
    for(int i = 0; i< resolution + 1; i++){
        // t continuously ranges from 0 to 1
        float t = float(i)/float(resolution);
        // HW4: Your code goes here.
        curve -> addPoint(deCasteljau(control -> P, t));
    }
}
void Spline::BSpline( ControlCurve* control, Curve* curve, int resolution ){
    int n = control -> size();
    if (n>=4){ // We only do BSpline when there are at least 4 control points
        for(int i = 0; i< resolution + 1; i++){
            // t continuously ranges from 1 to n-2
            float t = 1.f + float(n-3) * float(i)/float(resolution);
            
            float k = floor(t);

            // HW4: Your code goes here
            if (k >= 1 && k < n - 2) {
                curve->addPoint(BSplineHelper(
                    control->P[k - 1],
                    control->P[k],
                    control->P[k + 1],
                    control->P[k + 2],
                    t - k + 2));
            }

        }
    }
}

vec2s RecursiveSubdiv(vec2s points, int subdivLevel) {
    vec2s leftPoints = points;
    vec2s rightPoints;

    if (subdivLevel > 0) {
        leftPoints = SubdivLeft(points); //Get subdivided points of left half
        rightPoints = SubdivRight(points); //Get subdivided points of right half

        leftPoints = RecursiveSubdiv(leftPoints, subdivLevel - 1); //Get subdivided points of left half's left half
        rightPoints = RecursiveSubdiv(rightPoints, subdivLevel - 1); //Get subdivided points of right half's right half
    }

    //Gather all subdivided points
    leftPoints.insert(leftPoints.end(), rightPoints.begin(), rightPoints.end());

    return leftPoints;
}


void Spline::Subdiv( ControlCurve* control, Curve* curve, int subdivLevel ){
    // HW4: Your code goes here
    // HW4: This part requires more planning!  The result of subdivision should
    //      converge to the Bezier curve.
    //      You can design a recursion.  Or you can write for loops that subdivide
    //      the correct set of curve segments at each level.
    // HW4: If subdivLevel = k, and control->size() = n.  Then curve is expected
    //      to have size n*(2^k).

    curve->P = RecursiveSubdiv(control->P, subdivLevel);
}