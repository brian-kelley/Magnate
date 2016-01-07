#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <iostream>
#include <vector>
#include "SdlHeaders.h"
#include "DebugTools.h"

class Shaders
{
public:
    Shaders();
    ~Shaders();
    GLuint getViewMatLoc();
    GLuint getProjMatLoc();
    GLuint getProgramID();
private:
    GLuint programID;
    GLuint vshadeID;
    GLuint fshadeID;
    GLuint viewLoc;
    GLuint projLoc;
};

#endif