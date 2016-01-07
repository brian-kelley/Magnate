#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Input.h"
#include "SdlHeaders.h"
#include "GlmHeaders.h"
#include "Constants.h"
#include "Broadcaster.h"
#include "Coord.h"
#include "Heightmap.h"

struct FrustumCorners
{
    FrustumCorners() {}
    FrustumCorners(glm::vec4 upperLeft, glm::vec4 upperRight, glm::vec4 lowerRight, glm::vec4 lowerLeft) : upperLeft(upperLeft), upperRight(upperRight), lowerRight(lowerRight), lowerLeft(lowerLeft) {}
    glm::vec4 upperLeft;
    glm::vec4 upperRight;
    glm::vec4 lowerRight;
    glm::vec4 lowerLeft;
};

class Camera
{
public:
    Camera(Heightmap* worldHeights);
    void update();
    glm::mat4 getViewMatrix();
    Broadcaster<glm::mat4> cameraMotion;    //broadcasts view matrix after updates
    FrustumCorners getFrustumCorners;
private:
    static void processWheel(const SDL_MouseWheelEvent& event); //handles scroll events
    static Camera* instance;
    void camFwd(Height worldH);
    void camLeft(Height worldH);
    void camBack(Height worldH);
    void camRight(Height worldH);
    void camCCW();
    void camCW();
    void zoomIn();
    void zoomOut();
    glm::vec3 camDir;
    glm::vec3 camPos;
    glm::vec3 camUp;
    glm::mat4 viewMat;
    glm::mat4 projMat;
    float camAngle;
    float camPitch;
    Heightmap* worldHeights;
    const float FOV = M_PI_4;
    const float NEAR = 1000;
    const float FAR = 0.01;
};

#endif