#ifndef __RENDERER__INCLUDED__
#define __RENDERER__INCLUDED__

#include <vector>
#include <string>
#include <iostream>
#include "Constants.h"
#include "Coord.h"
#include "GlmHeaders.h"
#include "SdlHeaders.h"
#include "DebugTools.h"
#include "UIRenderer.h"
#include "VBOManager.h"
#include "Shaders.h"
#include "Window.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void enableTexture();
    void disableTexture();
    void update2DMatrices();                 //only call during init
    void update3DMatrices();                 //when FOV or window changes
    void uploadMatrices(int dims); //before draw call, 2 or 3
    void updateWindowSize();
    void update();
private:
    Window window;
    Shaders shaders;
    Atlas mainAtlas;
    WorldRenderer worldRend;
    UIRenderer guiRend;
    ImmediateDraw imm;
    VBOManager vboMan;
    void initShaders();
    void addQuadVertex();
    void addLineVertex();
    void prepareFrame();
    void finalizeFrame();
    //data to support immediate-mode usage with a stream VBO
    bool textureOn;
    GLuint projLoc;
    GLuint viewLoc;
    GLuint modelLoc;
    //configuration variables
}
#endif