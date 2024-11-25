#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// HW2: You can add more helper functions if you want!

glm::mat3 rotation(const float degrees,const glm::vec3 axis){
    const float angle = degrees * M_PI/180.0f; // convert to radians

    //Calculate new rotation via Rodrigues formula
    glm::mat3 i(1.0f); //3x3 identity matrix
    
    glm::mat3 ax = glm::mat3(
        glm::vec3(0, -(axis.z), axis.y),
        glm::vec3(axis.z, 0, -(axis.x)),
        glm::vec3(-(axis.y), axis.x, 0));

    glm::mat3 rotMat = i + ax * sin(angle) + (ax * ax) * (1 - cos(angle)); //Rodrigues formula

    return rotMat;
}

void Camera::rotateRight(const float degrees){
    // HW2: Update the class members "eye," "up"
    up = up / length(up);
    eye = eye - target;
    eye = eye * rotation(degrees, up);
    eye = eye + target;
}
void Camera::rotateUp(const float degrees){
    // HW2: Update the class members "eye," "up
    up = up / length(up);
    glm::vec3 axis = glm::cross(target - eye, up);
    axis = axis / length(axis);
    up = up - glm::dot(axis, up) * axis;
    up = up / length(up);
    eye = eye - target;
    eye = rotation(degrees, axis) * eye;
    eye = eye + target;
    up = rotation(degrees, axis) * up;
}
void Camera::computeMatrices(){
    // Note that glm matrix column majored.
    // That is, A[i] is the ith column of A,
    // and A_{ij} in math notation is A[j][i] in glm notation.
    
    // HW2: Update the class member "view" for the view matrix using "eye," "target," "up."
    glm::vec3 z = (eye - target) / distance(eye, target);
    up = up - (glm::dot(z, up) * z);
    up = up / length(up);
    glm::vec3 x = glm::cross(up, z);
    glm::mat4 c = glm::mat4(
        x.x, x.y, x.z, 0,
        up.x, up.y, up.z, 0,
        z.x, z.y, z.z, 0,
        eye.x, eye.y, eye.z, 1);

    view = glm::inverse(c);
    
    // HW2: Update the class member "proj" for the perspective matrix using the class members "aspect," "fovy," "near," "far."
    float fovy_rad = fovy * M_PI/180.0f; // remember to convert degrees to radians.
    glm::mat4 perspective = glm::mat4(
        (1 / (aspect * tan(fovy_rad/2))), 0, 0, 0,
        0, (1 / tan(fovy_rad/2)), 0, 0,
        0, 0, -((far + near) / (far - near)), -1,
        0, 0, -2 * ((far * near) / (far - near)), 0);

    proj = perspective; // to be replaced
    
}

void Camera::reset(){
    eye = eye_default;// position of the eye
    target = target_default;  // look at target
    up = up_default;      // up vector
    fovy = fovy_default;  // field of view in degrees
    aspect = aspect_default; // aspect ratio
    near = near_default; // near clipping distance
    far = far_default; // far clipping distance
}