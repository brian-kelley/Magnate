#include "Camera.h"

using namespace glm;
using namespace constants;

vec3 Camera::camDir;
vec3 Camera::camUp;
vec3 Camera::camPos;
float Camera::camAngle;

void Camera::camInit()
{
    camPos = {0, 20, 0};
    camAngle = 0;
    getViewMatrix(); //discard result, this updates other vectors (up & at)
}

glm::mat4 Camera::getViewMatrix()
{
    vec3 at = {camPos.x + sin(camAngle) * cos(camPitch), camPos.y - 1, camPos.z + cos(camAngle) * cos(camPitch)};
    vec3 camUp = {sin(camPitch) * sin(camAngle), cos(camPitch), sin(camPitch) * cos(camAngle)};
    camUp = normalize(camUp);
    camDir = normalize(at - camPos);
    return lookAt(camPos, at, camUp);
}

void Camera::camFwd()
{
    //get y component out of camDir
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir *= (PAN_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camLeft()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    //rotate pi/2 to the left
    lookDir = {lookDir.y, -lookDir.x};
    lookDir *= (PAN_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camRight()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir = {-lookDir.y, lookDir.x};
    lookDir *= (PAN_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camBack()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir *= -(PAN_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camCCW() //positive radians (CAM_ROTATE_SPEED > 0)
{
    camDir = rotate(camDir, CAM_ROTATE_SPEED, {0, 1, 0});
    camAngle += CAM_ROTATE_SPEED;
}

void Camera::camCW() //negative radians
{
    camDir = rotate(camDir, -CAM_ROTATE_SPEED, {0, 1, 0});
    camAngle -= CAM_ROTATE_SPEED;
}