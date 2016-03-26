#include "Camera.h"

using namespace glm;

Broadcaster<glm::mat4> Camera::cameraMotion;
vec3 Camera::camDir;
vec3 Camera::camPos;
vec3 Camera::camUp;
mat4 Camera::viewMat;
mat4 Camera::projMat;
float Camera::camAngle;
float Camera::camPitch;
const float Camera::ROTATE_SPEED = M_PI;
const float Camera::MOVE_SPEED = 1.2;
const float Camera::ZOOM_SPEED = 0.16;
const float Camera::FOV = M_PI_4;
const float Camera::NEAR = 2;
const float Camera::FAR = 10000;
const Heightmap* Camera::heights = nullptr;

void Camera::init()
{
    camPos = {0, 20, 0};
    camAngle = 0;
    camPitch = M_PI_2 * 0.8;
    Input::wheelBroadcaster.addListener(NULL, processWheel);
    heights = &World::getHeights();      //simple handle for world's height data
}

void Camera::update()
{
    //get the world's height directly under camera
    //use Coord to get tile position
    Pos2 loc = Coord::worldToTile(vec4(camPos, 1));
    short worldH = heights->get(loc);
    bool updated = false;
    if(Input::keystate[SDL_SCANCODE_W])
    {
        camFwd(worldH);
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_A])
    {
        camLeft(worldH);
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_S])
    {
        camBack(worldH);
        updated = true;
    }
    if(Input::keystate[SDL_SCANCODE_D])
    {
        camRight(worldH);
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

glm::mat4 Camera::getViewMatrix()
{
    vec3 at = {camPos.x + sin(camAngle) * cos(camPitch), camPos.y - 1, camPos.z + cos(camAngle) * cos(camPitch)};
    vec3 camUp = {sin(camPitch) * sin(camAngle), cos(camPitch), sin(camPitch) * cos(camAngle)};
    camUp = normalize(camUp);
    camDir = normalize(at - camPos);
    PRINT("Cam looking in dir: " << camDir);
    viewMat = lookAt(camPos, at, camUp);
    return viewMat;
}

glm::mat4 Camera::getProjMatrix(int winW, int winH)
{
    projMat = perspectiveFov<float>(FOV, winW, winH, FAR, NEAR);
    return projMat;
}

void Camera::camFwd(short worldH)
{
    //get y component out of camDir
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= (Input::dt * MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camLeft(short worldH)
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    //rotate pi/2 to the left
    lookDir = {lookDir.y, -lookDir.x};
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= (Input::dt * MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camRight(short worldH)
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir = {-lookDir.y, lookDir.x};
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= (Input::dt * MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camBack(short worldH)
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= -(Input::dt * MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camCCW() //positive radians (CAM_ROTATE_SPEED > 0)
{
    camDir = rotate(camDir, float(Input::dt * ROTATE_SPEED), {0, 1, 0});
    camAngle += Input::dt * ROTATE_SPEED;
}

void Camera::camCW() //negative radians
{
    camDir = rotate(camDir, float(Input::dt * -ROTATE_SPEED), {0, 1, 0});
    camAngle -= Input::dt * ROTATE_SPEED;
}

void Camera::zoomIn()
{
    short worldH = heights->get(Coord::worldToTile(vec4(camPos, 1)));
    camPos.y -= ZOOM_SPEED * (camPos.y - worldH * Coord::TERRAIN_Y_SCALE);
}

void Camera::zoomOut()
{
    short worldH = heights->get(Coord::worldToTile(vec4(camPos, 1)));
    camPos.y += ZOOM_SPEED * (camPos.y - worldH * Coord::TERRAIN_Y_SCALE);
}

void Camera::processWheel(void*, const SDL_MouseWheelEvent &event)
{
    if(event.y > 0)
        zoomIn();
    else if(event.y < 0)
        zoomOut();
}

void Camera::moveToPos(glm::vec4 pos)
{
    camPos = pos.xyz();
    cameraMotion.send(getViewMatrix());
}

vec3 Camera::getPosition()
{
    return camPos;
}
 
FrustumCorners Camera::getFrustumCorners(const glm::mat4 &view, const glm::mat4 &proj)
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
