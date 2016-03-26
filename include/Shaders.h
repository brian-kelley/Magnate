#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <iostream>
#include <vector>
#include "SdlHeaders.h"
#include "DebugTools.h"
#include "VBO.h"

class Shaders
{
public:
    Shaders();
    ~Shaders();
    GLint programID;
    GLint vshadeID;
    GLint fshadeID;
    GLint modelLoc;
    GLint viewLoc;
    GLint projLoc;
};

#endif
