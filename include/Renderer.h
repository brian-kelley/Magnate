#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <vector>
#include <string>
#include <iostream>
#include "World.h"
#include "Coord.h"
#include "GlmHeaders.h"
#include "SdlHeaders.h"
#include "DebugTools.h"
#include "WorldRenderer.h"
#include "UIRenderer.h"
#include "Models.h"
#include "VBO.h"
#include "Shaders.h"
#include "Window.h"
#include "GlobalConfig.h"

class Renderer
{
public:
    Renderer();
    void upload2DMatrices();                 //only call during init
    void upload3DMatrices();                 //when FOV or window changes
    void uploadMatrices(int dims); //before draw call, 2 or 3
    void update();
    void modelTest();
private:
    Window win;
    Shaders shaders;
    WorldRenderer worldRend;
    UIRenderer uiRend;
    GLuint modelLoc;
    GLuint projLoc;
    GLuint viewLoc;
};
#endif
