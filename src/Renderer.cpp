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
    VBO::loadAttribLocs(shaders.getProgramID());
    Atlas::init("main", win.getRenderer());
}

void Renderer::update()
{
    //Draw the world first, if in game
    upload3DMatrices();
    worldRend.draw();
    //Then draw GUI
    upload2DMatrices();
    uiRend.draw();
}

void Renderer::upload2DMatrices()
{
    auto winSize = win.getSize();
    mat4 proj2 = ortho<float>(0, winSize.x, winSize.y, 0, 1, -1);
    mat4 view2 = glm::mat4(); //ID mat: (0,0,0) is camera origin
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view2));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj2));
}

void Renderer::upload3DMatrices()
{
    auto winSize = win.getSize();
    mat4 proj3 = Camera::getProjMatrix(winSize.x, winSize.y);
    mat4 view3 = Camera::getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view3));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj3));
}