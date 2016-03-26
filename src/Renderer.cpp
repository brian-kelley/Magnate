#include "Renderer.h"

using namespace std;
using namespace glm;
using namespace Coord;

Renderer::Renderer() :
win(640, 480),
shaders(),
worldRend(),
uiRend(),
modelRend(shaders.modelLoc)
{
    auto progID = shaders.programID;
    modelLoc = glGetUniformLocation(progID, "model");
    viewLoc = glGetUniformLocation(progID, "view");
    projLoc = glGetUniformLocation(progID, "proj");
    //set the sunlight vector (TODO: do somewhere else)
    auto sunLoc = glGetUniformLocation(progID, "sunlight");
    vec3 sunlight(0.4, -0.8, 0.2);
    sunlight = normalize(sunlight);
    glUniform3fv(sunLoc, 1, value_ptr(sunlight));
    Atlas::init("main");
}

void Renderer::update()
{
    win.prepareFrame();
    //Draw the world first, if in game
    //Use nearest neighbor texture sampling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    upload3DMatrices();
    worldRend.draw();
    //Model renderer test
    modelTest();
    GLERR
    //Then draw GUI (function computes vertices and does draw calls)
    //Use bilinear for this
    upload2DMatrices();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    uiRend.draw();
    //Finally, flip window
    glFlush();
    win.endFrame();
}

void Renderer::upload2DMatrices()
{
    GLERR
    auto winSize = win.getSize();
    mat4 proj2 = ortho<float>(0, winSize.x, winSize.y, 0, -1, 1);
    mat4 view2 = glm::mat4(); //ID mat: (0,0,0) is camera origin
    mat4 model;
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(view2));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view2));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj2));
}

void Renderer::upload3DMatrices()
{
    auto winSize = win.getSize();
    mat4 proj3 = Camera::getProjMatrix(winSize.x, winSize.y);
    mat4 view3 = Camera::getViewMatrix();
    mat4 model3 = glm::mat4();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model3));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view3));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj3));
}

void Renderer::modelTest()
{
    glm::mat4 mat;
    glm::vec3 scale(10, 10, 10);
    mat = glm::scale(mat, scale);
    //todo: test scale/rotate/translate
    modelRend.drawModel("sphereTank", mat);
}
