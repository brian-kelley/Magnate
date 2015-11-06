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
    void camFwd(short worldH = 0);
    void camBack(short worldH = 0);
    void camLeft(short worldH = 0);
    void camRight(short worldH = 0);
    void zoomIn(short worldH = 0);
    void zoomOut(short worldH = 0);
    void camCW();
    void camCCW();
}

#endif