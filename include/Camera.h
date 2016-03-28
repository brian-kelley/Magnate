#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Input.h"
#include "SdlHeaders.h"
#include "GlmHeaders.h"
#include "Broadcaster.h"
#include "Coord.h"
#include "Heightmap.h"
#include "World.h"
#include "DebugTools.h"
#include "Window.h"

struct FrustumCorners
{
    glm::vec4 upperLeft;
    glm::vec4 upperRight;
    glm::vec4 lowerRight;
    glm::vec4 lowerLeft;
};

//Keeps track of camera position and orientation in world
//Moves based on input
class Camera
{
public:
    static void init();
    static void update();
    static void moveToPos(glm::vec4 pos);          //translate camera to position in world
    static glm::mat4 getViewMatrix();
    static glm::mat4 getProjMatrix(int winW, int winH);
    static FrustumCorners getFrustumCorners(const glm::mat4& view, const glm::mat4& proj);
    static glm::vec3 getPosition();
    static Broadcaster<glm::mat4> cameraMotion;    //broadcasts view matrix after updates
    static glm::mat4 viewMat;
    static glm::mat4 projMat;
    static glm::vec3 getWorldIntersect(int winX, int winY);             //get first intersection of window pixel with world mesh
    static glm::vec3 getWorldIntersect(glm::vec3 head, glm::vec3 dir);  //get first intersection of worldspace vector
private:
    static const Heightmap* heights;
    static void processWheel(void*, const SDL_MouseWheelEvent& event); //handles scroll events
    static void camFwd(short worldH);
    static void camLeft(short worldH);
    static void camBack(short worldH);
    static void camRight(short worldH);
    static void camCCW();
    static void camCW();
    static void zoomIn();
    static void zoomOut();
    static glm::vec3 camDir;
    static glm::vec3 camPos;
    static glm::vec3 camUp;
    static float camAngle;
    static float camPitch;
    static const float ROTATE_SPEED; //full circle in 2 seconds
    static const float MOVE_SPEED;          //multiplied by current height above ground
    static const float ZOOM_SPEED;
    static const float FOV;
    static const float NEAR;
    static const float FAR;
};

#endif
