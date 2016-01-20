#include "Renderer.h"

using namespace std;
using namespace glm;
using namespace Coord;

Renderer::Renderer() :
win(640, 480),
shaders(),
worldRend(),
uiRend()
{
    viewLoc = shaders.getViewMatLoc();
    projLoc = shaders.getProjMatLoc();
    Atlas::init("main", win.getRenderer());
}

void Renderer::update()
{
    win.prepareFrame();
    //Draw the world first, if in game
    GLERR
    upload3DMatrices();
    GLERR
    //worldRend.draw();
    //Then draw GUI (function computes vertices and does draw calls)
    upload2DMatrices();
    GLERR
    uiRend.draw();
    GLERR
    //Finally, flip window
    win.endFrame();
}

void Renderer::upload2DMatrices()
{
    GLERR
    auto winSize = win.getSize();
    mat4 proj2 = ortho<float>(0, winSize.x, winSize.y, 0, -1, 1);
    mat4 view2 = glm::mat4(); //ID mat: (0,0,0) is camera origin
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view2));
    GLERR
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj2));
    GLERR
}

void Renderer::upload3DMatrices()
{
    auto winSize = win.getSize();
    mat4 proj3 = Camera::getProjMatrix(winSize.x, winSize.y);
    mat4 view3 = Camera::getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view3));
    GLERR
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj3));
    GLERR
}