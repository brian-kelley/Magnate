#include "Camera.h"

using namespace glm;
using namespace Coord;

namespace Camera
{

Broadcaster<glm::mat4> cameraMotion;
vec3 camDir;
vec3 camPos;
vec3 camUp;
mat4 viewMat;
mat4 projMat;
float camAngle;
float camPitch;
const float ROTATE_SPEED = M_PI;
const float MOVE_SPEED = 1.2;
const float ZOOM_SPEED = 0.16;
const float FOV = (M_PI_4 + M_PI_4 / 2);
const float NEAR = 0.4;
const float FAR = 500;


void init()
{
    camPos = {0, 20, 0};
    camAngle = 0;
    camPitch = M_PI_2 * 0.8;
    Input::wheelBroadcaster.addListener(NULL, processWheel);
}

void update()
{
    //get the world's height directly under camera
    //use Coord to get tile position
    bool updated = false;
    if(Input::keystate[SDL_SCANCODE_W])
    {
        camFwd();
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_A])
    {
        camLeft();
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_S])
    {
        camBack();
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_D])
    {
        camRight();
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_Q])
    {
        camCCW();
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_E])
    {
        camCW();
        updated = true;
    }
    if(updated)
    {
        cameraMotion.send(getViewMatrix());
    }
}

glm::mat4 getViewMatrix()
{
    vec3 at = {camPos.x + sin(camAngle) * cos(camPitch), camPos.y - 1, camPos.z + cos(camAngle) * cos(camPitch)};
    vec3 camUp = {sin(camPitch) * sin(camAngle), cos(camPitch), sin(camPitch) * cos(camAngle)};
    camUp = normalize(camUp);
    camDir = normalize(at - camPos);
    viewMat = lookAt(camPos, at, camUp);
    return viewMat;
}

glm::mat4 getProjMatrix(int winW, int winH)
{
    projMat = perspectiveFov<float>(FOV, winW, winH, FAR, NEAR);
    //do a test
    //getWorldIntersect(0, 0);
    return projMat;
}

Pos2 worldToScreen(vec3& worldPos)
{
    vec4 pos4 = vec4(worldPos, 1);
    pos4 = viewMat * pos4;
    pos4 = projMat * pos4;
    return Pos2(0.5 * (1 + pos4.x), 0.5 * (1 - pos4.y));
}

void camFwd()
{
    //get y component out of camDir
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir *= (Input::dt * MOVE_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void camLeft()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    //rotate pi/2 to the left
    lookDir = {lookDir.y, -lookDir.x};
    lookDir *= (Input::dt * MOVE_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void camRight()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir = {-lookDir.y, lookDir.x};
    lookDir *= (Input::dt * MOVE_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void camBack()
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir *= -(Input::dt * MOVE_SPEED * camPos.y);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void camCCW() //positive radians (CAM_ROTATE_SPEED > 0)
{
    camDir = rotate(camDir, float(Input::dt * ROTATE_SPEED), {0, 1, 0});
    camAngle += Input::dt * ROTATE_SPEED;
}

void camCW() //negative radians
{
    camDir = rotate(camDir, float(Input::dt * -ROTATE_SPEED), {0, 1, 0});
    camAngle -= Input::dt * ROTATE_SPEED;
}

void zoomIn()
{
    camPos.y -= ZOOM_SPEED * camPos.y;
}

void zoomOut()
{
    camPos.y += ZOOM_SPEED * camPos.y;
}

void processWheel(void*, const SDL_MouseWheelEvent &event)
{
    if(event.y > 0)
        zoomIn();
    else if(event.y < 0)
        zoomOut();
}

void moveToPos(glm::vec4 pos)
{
    camPos = pos.xyz();
    cameraMotion.send(getViewMatrix());
}
 
FrustumCorners getFrustumCorners(const glm::mat4 &view, const glm::mat4 &proj)
{
    FrustumCorners corners;
    //Calculate camera at position from view matrix (is origin in camera space)
    mat4 projInv = glm::inverse(proj);
    mat4 viewInv = glm::inverse(view);
    float back = 1;
    float front = -1;
    float left = -1;
    float right = 1;
    float up = 1;
    float down = -1;
    vec4 lub = {left, up, back, 1};
    vec4 rub = {right, up, back, 1};
    vec4 rdb = {right, down, back, 1};
    vec4 ldb = {left, down, back, 1};
    vec4 luf = {left, up, front, 1};
    vec4 ruf = {right, up, front, 1};
    vec4 rdf = {right, down, front, 1};
    vec4 ldf = {left, down, front, 1};
    lub = projInv * lub;
    rub = projInv * rub;
    rdb = projInv * rdb;
    ldb = projInv * ldb;
    luf = projInv * luf;
    ruf = projInv * ruf;
    rdf = projInv * rdf;
    ldf = projInv * ldf;
    lub /= lub.w;
    rub /= rub.w;
    rdb /= rdb.w;
    ldb /= ldb.w;
    luf /= luf.w;
    ruf /= ruf.w;
    rdf /= rdf.w;
    ldf /= ldf.w;
    lub = viewInv * lub;
    rub = viewInv * rub;
    rdb = viewInv * rdb;
    ldb = viewInv * ldb;
    luf = viewInv * luf;
    ruf = viewInv * ruf;
    rdf = viewInv * rdf;
    ldf = viewInv * ldf;
    //frustum corner points are now world space
    //draw line between front and back point pairs and find where y = 0
    vec4 luVec = normalize(luf - lub);
    vec4 ruVec = normalize(ruf - rub);
    vec4 rdVec = normalize(rdf - rdb);
    vec4 ldVec = normalize(ldf - ldb);
    float luMult = -lub.y / luVec.y;
    float ruMult = -rub.y / ruVec.y;
    float rdMult = -rdb.y / rdVec.y;
    float ldMult = -ldb.y / ldVec.y;
    corners.upperLeft = {camPos.x + luMult * luVec.x, 0, camPos.z + luMult * luVec.z, 1};
    corners.upperRight = {camPos.x + ruMult * ruVec.x, 0, camPos.z + ruMult * ruVec.z, 1};
    corners.lowerRight = {camPos.x + rdMult * rdVec.x, 0, camPos.z + rdMult * rdVec.z, 1};
    corners.lowerLeft = {camPos.x + ldMult * ldVec.x, 0, camPos.z + ldMult * ldVec.z, 1};
    return corners;
}

vec3 getWorldIntersect(int winX, int winY)
{
    //transform point and direction to worldspace
    vec4 tail = {winX, winY, 2, 1};
    vec4 head = {winX, winY, 1, 1};
    //convert head from viewport to clip
    head.x *= (2.0 / Window::w);
    head.y *= (2.0 / Window::h);
    tail.x *= (2.0 / Window::w);
    tail.y *= (2.0 / Window::h);
    //convert head and dir from clip to view
    mat4 invProj = inverse(projMat);
    head = invProj * head;
    tail = invProj * tail;
    head /= head.w;
    tail /= tail.w;
    //convert head and dir from view to world
    mat4 invView = inverse(viewMat);
    head = invView * head;
    tail = invView * tail;
    return getWorldIntersect(head, head - tail);
}

vec3 getWorldIntersect(vec4 head, vec4 dir)
{
    //get sea level intersection point
    vec2 seaLevel = (head - dir * (head.y / dir.y)).xz();
    Pos2 tileStart = worldToTile(head);
    vec2 iter = tileToWorld(tileStart.x, 0, tileStart.y).xz();
    return vec3();
}

}
