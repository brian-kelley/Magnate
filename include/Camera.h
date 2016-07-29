#ifndef CAMERA_H
#define CAMERA_H

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
namespace Camera
{
     void init();
     void update();
     void moveToPos(vec4 pos);          //translate camera to position in world
     glm::mat4 getViewMatrix();
     glm::mat4 getProjMatrix(int winW, int winH);
     FrustumCorners getFrustumCorners(const glm::mat4& view, const glm::mat4& proj);
     Pos2 worldToScreen(vec3& worldPos);
     vec3 getWorldIntersect(int winX, int winY);             //get first intersection of window pixel with world mesh
     vec3 getWorldIntersect(vec4 head, vec4 dir);  //get first intersection of worldspace vector
     void processWheel(void*, const SDL_MouseWheelEvent& event); //handles scroll events
     void camFwd();
     void camLeft();
     void camBack();
     void camRight();
     void camCCW();
     void camCW();
     void zoomIn();
     void zoomOut();

     extern Broadcaster<glm::mat4> cameraMotion;    //broadcasts view matrix after updates
     extern glm::mat4 viewMat;
     extern glm::mat4 projMat;
     extern vec3 camDir;
     extern vec3 camPos;
     extern vec3 camUp;
     extern float camAngle;
     extern float camPitch;
     extern const float ROTATE_SPEED; //full circle in 2 seconds
     extern const float MOVE_SPEED;          //multiplied by current height above ground
     extern const float ZOOM_SPEED;
     extern const float FOV;
     extern const float NEAR;
     extern const float FAR;
};

#endif
