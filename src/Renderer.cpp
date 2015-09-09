#include "Renderer.h"

using namespace std;
using namespace glm;
using namespace constants;
using namespace Coord;

bool Renderer::textureOn;
GLuint Renderer::programID;
GLuint Renderer::vshadeID;
GLuint Renderer::fshadeID;
GLuint Renderer::posAttribLoc;
GLuint Renderer::colorAttribLoc;
GLuint Renderer::texCoordAttribLoc;
int Renderer::worldVBOSize;
int Renderer::buildingVBOSize;
int Renderer::guiVBOSize;
Vertex2D Renderer::stateVertex;
mat4 Renderer::proj2;
mat4 Renderer::view2;
GLuint Renderer::projLoc;
GLuint Renderer::viewLoc;
GLuint Renderer::worldVBO;
GLuint Renderer::buildingVBO;
GLuint Renderer::guiVBO;
int Renderer::numWorldVertices;
int Renderer::numBuildingVertices;
int Renderer::numGuiQuadVertices;
int Renderer::numGuiLineVertices;
vector<Vertex2D> Renderer::guiQuadVertices;
vector<Vertex2D> Renderer::guiLineVertices;

void Renderer::init()
{
    textureOn = false;
    initShaders();
    //For now, model matrix in both dimensions is I
    stateVertex.pos = {0, 0};
    //    stateVertex.z = 0;
    stateVertex.texcoord = {0, 0};
    stateVertex.color = {255, 255, 255, 255};
    //Set initial camera position and direction
    Camera::camInit();
    update2DMatrices();
    update3DMatrices();
    initVBO();
    numWorldVertices = VBO_CHUNKS * CHUNK_SIZE * CHUNK_SIZE * 4;
}

void Renderer::initVBO()
{
    guiQuadVertices.reserve(GUI_QUAD_PRELOAD);
    guiLineVertices.reserve(GUI_LINE_PRELOAD);
    //Set up vertex attributes for shader, which are the same
    //for all 3 VBOs
    posAttribLoc = glGetAttribLocation(programID, "vertex");
    colorAttribLoc = glGetAttribLocation(programID, "color");
    texCoordAttribLoc = glGetAttribLocation(programID, "texCoord");
    glEnableVertexAttribArray(posAttribLoc);
    glEnableVertexAttribArray(colorAttribLoc);
    glEnableVertexAttribArray(texCoordAttribLoc);
    setupWorldVBO();
    setupBuildingVBO();
    setupGuiVBO();
}

void Renderer::setupWorldVBO()
{
    glGenBuffers(1, &worldVBO);
    glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
    GLERR
    int worldVBOSize = VBO_CHUNKS * CHUNK_SIZE * CHUNK_SIZE * sizeof(Vertex3D) * 4;
    PRINT("Creating world vbo with " << worldVBOSize << " bytes.")
    glBufferData(GL_ARRAY_BUFFER, VBO_CHUNKS * CHUNK_SIZE * CHUNK_SIZE * sizeof(Vertex3D) * 4, NULL, GL_STATIC_DRAW);
    GLERR
}

void Renderer::setupBuildingVBO()
{
    glGenBuffers(1, &buildingVBO);
    glBindBuffer(GL_ARRAY_BUFFER, buildingVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_BUILDING_QUADS * 4 * sizeof(Vertex3D), nullptr, GL_DYNAMIC_DRAW);
}

void Renderer::setupGuiVBO()
{
    glGenBuffers(1, &guiVBO);
    glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
    guiVBOSize = MAX_GUI_QUADS * 4 + MAX_GUI_LINES * 2;
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * (guiVBOSize), NULL, GL_STREAM_DRAW);
}

void Renderer::dispose()
{
    glDetachShader(programID, vshadeID);
    glDetachShader(programID, fshadeID);
    glDeleteShader(vshadeID);
    glDeleteShader(fshadeID);
    glDeleteProgram(programID);
}

void Renderer::initShaders()
{
    vector<string> code = {
    "#version 120",
    "attribute vec3 vertex;",
    "attribute vec4 color;",
    "attribute vec2 texCoord;",
    "varying vec2 fragTexCoord;",
    "varying vec4 fragBaseColor;",
    "uniform mat4 view;",
    "uniform mat4 proj;",
    "uniform sampler2D sampler;",
    "void main()",
    "{",
    "    gl_Position = proj * view * vec4(vertex, 1.0);",
    "    fragTexCoord = texCoord;",
    "    fragBaseColor = color;",
    "}"};
    string vertexShadeCode = "";
    for(auto line : code)
    {
        vertexShadeCode += line + "\n";
    }
    code = {
    "#version 120",
    "uniform sampler2D sampler;",
    "varying vec2 fragTexCoord;",
    "varying vec4 fragBaseColor;",
    "void main()",
    "{",
    "    if(fragTexCoord.x == -1)",
    "    {",
    "        gl_FragColor = fragBaseColor;",
    "    }",
    "    else",
    "    {",
    "        vec2 normalTexCoord = fragTexCoord / 2048.0;",
    "        vec4 texelColor = texture2D(sampler, normalTexCoord);",
    "        gl_FragColor.x = texelColor.x * fragBaseColor.x;",
    "        gl_FragColor.y = texelColor.y * fragBaseColor.y;",
    "        gl_FragColor.z = texelColor.z * fragBaseColor.z;",
    "        gl_FragColor.w = texelColor.w * fragBaseColor.w;",
    "    }",
    "}"};
    string fragmentShadeCode = "";
    for(auto line : code)
    {
        fragmentShadeCode += line + "\n";
    }
    code.clear();
    const char* vcstr = vertexShadeCode.c_str();
    const char* fcstr = fragmentShadeCode.c_str();
    programID = glCreateProgram();
    vshadeID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshadeID, 1, &vcstr, NULL);
    glCompileShader(vshadeID);
    int ERR_BUF_LEN = 500;
    char error[ERR_BUF_LEN];
    error[0] = 0;
    glGetShaderInfoLog(vshadeID, 500, NULL, error);
    if(error[0])
    {
        cout << error << endl;
        throw runtime_error("Vertex shader compile error.");
    }
    fshadeID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshadeID, 1, &fcstr, NULL);
    glCompileShader(fshadeID);
    error[0] = 0;
    glGetShaderInfoLog(fshadeID, 500, NULL, error);
    if(error[0])
    {
        cout << error << endl;
        throw runtime_error("Fragment shader compile error.");
    }
    glAttachShader(programID, vshadeID);
    glAttachShader(programID, fshadeID);
    GLERR
    glLinkProgram(programID);
    GLERR
    error[0] = 0;
    GLuint linkSuccess;
    glGetProgramiv(programID, GL_LINK_STATUS, (int*) &linkSuccess);
    GLERR
    if(!linkSuccess)
    {
        glGetProgramInfoLog(programID, ERR_BUF_LEN, &ERR_BUF_LEN, error);
        cout << error << endl;
        throw runtime_error("Shader linking error.");
    }
    glUseProgram(programID);
    GLERR
    GLuint samplerLoc = glGetUniformLocation(programID, "sampler");
    GLERR
    glActiveTexture(GL_TEXTURE0);
    GLERR
    glUniform1i(samplerLoc, 0);
    GLERR
    viewLoc = glGetUniformLocation(programID, "view");
    projLoc = glGetUniformLocation(programID, "proj");
    GLERR
}

void Renderer::startFrame()
{
    numGuiLineVertices = 0;
    numGuiQuadVertices = 0;
}

void Renderer::endFrame()
{
    uploadMatrices(3);
    //draw world VBO
    bindWorldVBO();
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_QUADS, 0, numWorldVertices);
    //draw building VBO
    //bindBuildingVBO();
    //glDrawArrays(GL_QUADS, 0, 0);
    //draw GUI VBO
    bindGuiVBO();
    uploadMatrices(2);
    int totalVertices = numGuiQuadVertices + numGuiLineVertices;
    if(sizeof(Vertex2D) * totalVertices > guiVBOSize)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * totalVertices, NULL, GL_STREAM_DRAW);
        guiVBOSize = sizeof(Vertex2D) * totalVertices;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex2D) * numGuiQuadVertices, &guiQuadVertices.front());
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex2D) * numGuiQuadVertices, sizeof(Vertex2D) * numGuiLineVertices, &guiLineVertices.front());
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_QUADS, 0, numGuiQuadVertices);
    glDrawArrays(GL_LINES, numGuiQuadVertices, numGuiLineVertices);
}

void Renderer::color3f(float r, float g, float b)
{
    stateVertex.color.r = r * 255;
    stateVertex.color.g = g * 255;
    stateVertex.color.b = b * 255;
    stateVertex.color.a = 255;
}

void Renderer::color3b(unsigned char r, unsigned char g, unsigned char b)
{
    stateVertex.color.r = r;
    stateVertex.color.g = g;
    stateVertex.color.b = b;
    stateVertex.color.a = 255;
}

void Renderer::color4f(float r, float g, float b, float a)
{
    stateVertex.color.r = r * 255;
    stateVertex.color.g = g * 255;
    stateVertex.color.b = b * 255;
    stateVertex.color.a = a * 255;
}

void Renderer::color4b(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    stateVertex.color.r = r;
    stateVertex.color.g = g;
    stateVertex.color.b = b;
    stateVertex.color.a = a;
}

void Renderer::vertex2i(short x, short y)
{
    stateVertex.pos = {x, y};
    addQuadVertex();
}

void Renderer::lineVertex2i(short x, short y)
{
    stateVertex.pos = {x, y};
    addLineVertex();
}

void Renderer::texCoord2i(short u, short v)
{
    stateVertex.texcoord = {u, v};
    textureOn = true;
}

void Renderer::texCoord2f(float u, float v)
{
    short newU = u * ATLAS_SIZE;
    short newV = v * ATLAS_SIZE;
    stateVertex.texcoord = {newU, newV};
    textureOn = true;
}

void Renderer::addQuadVertex()
{
    if(!textureOn)
    {
        //Shaders will know that these coords mean to not use texture2D for color
        stateVertex.texcoord.u = -1;
    }
    //else: texcoord must have already been set by program
    if(guiQuadVertices.size() > numGuiQuadVertices)
    {
        guiQuadVertices[numGuiQuadVertices] = stateVertex;
    }
    else
    {
        guiQuadVertices.push_back(stateVertex);
    }
    numGuiQuadVertices++;
}

void Renderer::addLineVertex()
{
    //Shaders will know that these coords mean to not use texture2D for color
    //else: texcoord must have already been set by program
    stateVertex.texcoord.u = -1;
    if(guiLineVertices.size() > numGuiLineVertices)
    {
        guiLineVertices[numGuiLineVertices] = stateVertex;
    }
    else
    {
        guiLineVertices.push_back(stateVertex);
    }
    numGuiLineVertices++;
}

void Renderer::enableTexture()
{
    textureOn = true;
}

void Renderer::disableTexture()
{
    textureOn = false;
}

void Renderer::update2DMatrices()
{
    proj2 = ortho<float>(0, WINDOW_W, WINDOW_H, 0, 1, -1);
    view2 = glm::mat4(); //ID
}

void Renderer::update3DMatrices()
{
    Coord::proj3 = glm::perspective<float>(FOV, float(WINDOW_W) / WINDOW_H, NEAR, FAR);
    Coord::view3 = Camera::getViewMatrix();
}

void Renderer::uploadMatrices(int dims)
{
    if(dims == 2)
    {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view2));
        GLERR
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj2));
        GLERR
    }
    else
    {
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(Coord::view3));
        GLERR
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(Coord::proj3));
        GLERR
    }
}

void Renderer::bindWorldVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
    glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (GLvoid*) 0);
    glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 4);
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 8);
    GLERR
}

void Renderer::bindBuildingVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, buildingVBO);
    glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (GLvoid*) 0);
    glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 4);
    glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*) 8);
    GLERR
}

void Renderer::bindGuiVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
    glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (GLvoid*) 0);
    glVertexAttribPointer(texCoordAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 4);
    glVertexAttribPointer(posAttribLoc, 2, GL_SHORT, GL_FALSE, sizeof(Vertex2D), (GLvoid*) 8);
    GLERR
}

vec2 Renderer::xzPlaneIntersect(vec3 pos, vec3 dir)
{
    float mult = pos.y / dir.y;
    return {pos.x + dir.x * mult, pos.z + dir.z * mult};
}