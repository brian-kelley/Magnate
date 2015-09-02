#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "GlmHeaders.h"
#include "Constants.h"

namespace Camera
{
    extern glm::vec3 camDir;
    extern glm::vec3 camPos;
    extern glm::vec3 camUp;
    extern float camAngle;
    const float camPitch = M_PI_2 * 0.8;
    void camInit();
    glm::mat4 getViewMatrix();
    void camFwd();
    void camBack();
    void camLeft();
    void camRight();
    void camCW();
    void camCCW();
}

#endif