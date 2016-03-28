#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <iostream>
#include <vector>
#include "SdlHeaders.h"
#include "DebugTools.h"
#include "VBO.h"

namespace Shaders
{
    void init();
    void dispose();
    extern GLint programID;
    extern GLint vshadeID;
    extern GLint fshadeID;
    extern GLint modelLoc;
    extern GLint viewLoc;
    extern GLint projLoc;
};

#endif
