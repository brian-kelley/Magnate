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
const float Camera::ROTATE_SPEED = M_PI / 60.0;
const float Camera::MOVE_SPEED = 0.02;
const float Camera::ZOOM_SPEED = 0.16;
const float Camera::FOV = M_PI_4;
const float Camera::NEAR = 1000;
const float Camera::FAR = 0.01;
const Heightmap* Camera::heights = nullptr;

void Camera::init()
{
    camPos = {0, 20, 0};
    camAngle = 0;
    camPitch = M_PI_2 * 0.8;
    Input::wheelBroadcaster.addListener(nullptr, processWheel);
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
    return lookAt(camPos, at, camUp);
}

glm::mat4 Camera::getProjMatrix(int winW, int winH)
{
    return perspectiveFov<float>(FOV, winW, winH, NEAR, FAR);
}

void Camera::camFwd(short worldH)
{
    //get y component out of camDir
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= (MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camLeft(short worldH)
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    //rotate pi/2 to the left
    lookDir = {lookDir.y, -lookDir.x};
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= (MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camRight(short worldH)
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    lookDir = {-lookDir.y, lookDir.x};
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= (MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camBack(short worldH)
{
    vec2 lookDir = normalize(vec2(camDir.x, camDir.z));
    float heightOverGround = camPos.y - worldH * Coord::TERRAIN_Y_SCALE;
    lookDir *= -(MOVE_SPEED * heightOverGround);
    camPos.x += lookDir.x;
    camPos.z += lookDir.y;
}

void Camera::camCCW() //positive radians (CAM_ROTATE_SPEED > 0)
{
    camDir = rotate(camDir, ROTATE_SPEED, {0, 1, 0});
    camAngle += ROTATE_SPEED;
}

void Camera::camCW() //negative radians
{
    camDir = rotate(camDir, -ROTATE_SPEED, {0, 1, 0});
    camAngle -= ROTATE_SPEED;
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

void Camera::processWheel(void* ins, const SDL_MouseWheelEvent &event)
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