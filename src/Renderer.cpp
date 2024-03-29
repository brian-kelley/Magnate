#include "Renderer.h"

using namespace std;
using namespace glm;
using namespace Coord;

GLint Renderer::modelLoc = -1;
GLint Renderer::viewLoc = -1;
GLint Renderer::projLoc = -1;

void Renderer::init()
{
    Window::init(640, 480);
    Shaders::init();
    Atlas::init("main");
    WorldRenderer::init();
    ImmediateDraw::init(5000, 100);     //num quads, num lines
    ModelRenderer::init(Shaders::modelLoc);
    auto progID = Shaders::programID;
    modelLoc = glGetUniformLocation(progID, "model");
    viewLoc = glGetUniformLocation(progID, "view");
    projLoc = glGetUniformLocation(progID, "proj");
    //set the sunlight vector (TODO: do somewhere else)
    auto sunLoc = glGetUniformLocation(progID, "sunlight");
    vec3 sunlight(0.4, 0.8, 0.2);
    sunlight = normalize(sunlight);
    glUniform3fv(sunLoc, 1, value_ptr(sunlight));
}

void Renderer::update()
{
    Window::prepareFrame();
    //Draw the world first, if in game
    //Use nearest neighbor texture sampling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    upload3DMatrices();
    WorldRenderer::draw();
    //Model renderer test - not doing while working on mesh
    //modelTest();
    GLERR
    //Then draw GUI (function computes vertices and does draw calls)
    //Use bilinear for this
    upload2DMatrices();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    UIRenderer::draw();
    ImmediateDraw::draw();
    //Finally, flip window
    glFlush();
    Window::endFrame();
}

void Renderer::upload2DMatrices()
{
    GLERR
    mat4 proj2 = ortho<float>(0, Window::w, Window::h, 0, -1, 1);
    mat4 view2 = glm::mat4(); //ID mat: (0,0,0) is camera origin
    mat4 model;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view2));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj2));
}

void Renderer::upload3DMatrices()
{
    mat4 proj3 = Camera::getProjMatrix(Window::w, Window::h);
    mat4 view3 = Camera::getViewMatrix();
    mat4 model3 = glm::mat4();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model3));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view3));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj3));
}

void Renderer::modelTest()
{
    mat4 mat;
    vec3 scale(10, 10, 10);
    mat = glm::scale(mat, scale);
    //todo: test scale/rotate/translate
    ModelRenderer::drawModel("sphereTank", mat);
}
