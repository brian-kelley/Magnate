#include "Renderer.h"

using namespace std;
using namespace glm;
using namespace constants;

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
mat4 Renderer::proj3;
mat4 Renderer::view2;
mat4 Renderer::view3;
vec3 Renderer::camUp;
vec3 Renderer::camDir;
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

//list of chunks currently cached in VBO, mapped to the actual vbo
static ChunkAllocation chunkAlloc[constants::MAX_VBO_CHUNKS];

void Renderer::init()
{
    textureOn = false;
    initShaders();
    //For now, model matrix in both dimensions is I
    stateVertex.x = 0;
    stateVertex.y = 0;
    stateVertex.z = 0;
    stateVertex.u = 0;
    stateVertex.v = 0;
    stateVertex.r = 255;
    stateVertex.g = 255;
    stateVertex.b = 255;
    stateVertex.a = 255;
    update2DMatrices();
    updatePerspectiveMatrix(); //use current (default) camera coordinates and window dimensions
    updateViewMatrix(); //from Constants.h
    initVBO();
}

void Renderer::initVBO()
{
    guiQuadVertices.reserve(GUI_QUAD_PRELOAD);
    guiLineVertices.reserve(GUI_LINE_PRELOAD);
    //setupWorldVBO();
    //setupBuildingVBO();
    setupGuiVBO();
}

void Renderer::setupWorldVBO()
{
    glGenBuffers(1, &worldVBO);
    glBindBuffer(GL_ARRAY_BUFFER, worldVBO);
    GLERR
    glBufferData(GL_ARRAY_BUFFER, MAX_VBO_CHUNKS * (CHUNK_SIZE + 1) * (CHUNK_SIZE + 1) * sizeof(Vertex3D), NULL, GL_STATIC_DRAW);
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
    "    if(fragTexCoord.x == 0xFFFF || fragTexCoord.y == 0xFFFF)",
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
    glLinkProgram(programID);
    error[0] = 0;
    GLuint linkSuccess;
    glGetProgramiv(programID, GL_LINK_STATUS, (int*) &linkSuccess);
    if(!linkSuccess)
    {
        glGetProgramInfoLog(programID, ERR_BUF_LEN, &ERR_BUF_LEN, error);
        cout << error << endl;
        throw runtime_error("Shader linking error.");
    }
    glUseProgram(programID);
    GLuint samplerLoc = glGetUniformLocation(programID, "sampler");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(samplerLoc, 0);
    viewLoc = glGetUniformLocation(programID, "view");
    projLoc = glGetUniformLocation(programID, "proj");
}

void Renderer::startFrame()
{
    numGuiLineVertices = 0;
    numGuiQuadVertices = 0;
}

void Renderer::endFrame()
{
    /*
    uploadMatrices(3);
    //draw world VBO
    glBindVertexArrayAPPLE(worldVAO);
    //draw building VBO
    glBindVertexArrayAPPLE(buildingVAO);
    glDrawArrays(GL_QUADS, 0, numBuildingVertices);
     */
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
    glDrawArrays(GL_QUADS, 0, numGuiQuadVertices);
    glDrawArrays(GL_LINES, numGuiQuadVertices, numGuiLineVertices);
}

void Renderer::color3f(float r, float g, float b)
{
    stateVertex.r = 255 * r;
    stateVertex.g = 255 * g;
    stateVertex.b = 255 * b;
    stateVertex.a = 255;
}

void Renderer::color3b(unsigned char r, unsigned char g, unsigned char b)
{
    stateVertex.r = r;
    stateVertex.g = g;
    stateVertex.b = b;
    stateVertex.a = 255;
}

void Renderer::color4f(float r, float g, float b, float a)
{
    stateVertex.r = 255 * r;
    stateVertex.g = 255 * g;
    stateVertex.b = 255 * b;
    stateVertex.a = 255 * a;
}

void Renderer::color4b(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    stateVertex.r = r;
    stateVertex.g = g;
    stateVertex.b = b;
    stateVertex.a = a;
}

void Renderer::vertex2i(unsigned short x, unsigned short y)
{
    stateVertex.x = x;
    stateVertex.y = y;
    addQuadVertex();
}

void Renderer::lineVertex2i(int x, int y)
{
    stateVertex.x = x;
    stateVertex.y = y;
    addLineVertex();
}

void Renderer::texCoord2i(unsigned short u, unsigned short v)
{
    stateVertex.u = u;
    stateVertex.v = v;
    textureOn = true;
}

void Renderer::texCoord2f(float u, float v)
{
    stateVertex.u = u * ATLAS_SIZE;
    stateVertex.v = v * ATLAS_SIZE;
    textureOn = true;
}

void Renderer::addQuadVertex()
{
    if(!textureOn)
    {
        //Shaders will know that these coords mean to not use texture2D for color
        stateVertex.u = 0xFFFF;
        stateVertex.v = 0xFFFF;
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
    stateVertex.u = 0xFFFF;
    stateVertex.v = 0xFFFF;
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

void Renderer::updatePerspectiveMatrix()
{
    proj3 = perspectiveFov<float>(FOV, WINDOW_W, WINDOW_H, 0.1, 100);
}

void Renderer::updateViewMatrix()
{
    vec3 cam(camX, camY, camZ);
    cam = normalize(cam);
    vec3 at(camX + 10 * cos(camAngle) * sin(camPitch), camY - 10, camZ - 10 * sin(camAngle) * sin(camPitch));
    vec3 up(cos(camAngle) * cos(camPitch), cos(camPitch), -sin(camAngle) * cos(camPitch));
    camUp = up;
    camDir = normalize(at - cam);
    view3 = lookAt(cam, at, up);
}

void Renderer::uploadMatrices(int dims)
{
    if(dims == 2)
    {
        glUniformMatrix4fv(viewLoc, 1, false, value_ptr(view2));
        GLERR
        glUniformMatrix4fv(projLoc, 1, false, value_ptr(proj2));
        GLERR
    }
    else
    {
        glUniformMatrix4fv(viewLoc, 1, false, value_ptr(view3));
        GLERR
        glUniformMatrix4fv(projLoc, 1, false, value_ptr(proj3));
        GLERR
    }
}

void Renderer::getFrustumCorners(double *arr)
{
    //upper-left of screen - arr[0] = x, arr[1] = y/z
    double fovX = FOV; //angle between straight ahead and left or right of screen
    double fovY = 1.0 * fovX * WINDOW_H / WINDOW_W;
    vec3 camCross = cross(camUp, camDir);
    vec3 upperLeft = rotate<float>(camDir, fovX, camUp);
    upperLeft = rotate<float>(upperLeft, fovY, camCross);
    //get where x * upperLeft crosses y = 0, get (x, z) and store in arr[0], arr[1]
    arr[0] = camX + upperLeft.x * (camY / upperLeft.y);
    arr[1] = camZ + upperLeft.z * (camY / upperLeft.y);
    vec3 upperRight = rotate<float>(camDir, -fovX, camUp);
    upperRight = rotate<float>(upperRight, fovY, camCross);
    arr[2] = camX + upperRight.x * (camY / upperRight.y);
    arr[3] = camZ + upperRight.z * (camY / upperRight.y);
    vec3 camReverseNormal = -camUp;
    vec3 lowerLeft = reflect(upperLeft, camReverseNormal);
    vec3 lowerRight = reflect(upperRight, camReverseNormal);
    arr[4] = camX + lowerRight.x * (camY / upperRight.y);
    arr[5] = camZ + lowerRight.z * (camY / upperRight.y);
    arr[6] = camX + lowerLeft.x * (camY / upperRight.y);
    arr[7] = camZ + lowerLeft.z * (camY / upperRight.y);
}

void Renderer::bindWorldVBO()
{
    
}

void Renderer::bindBuildingVBO()
{
    
}

void Renderer::bindGuiVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
    posAttribLoc = glGetAttribLocation(programID, "vertex");
    colorAttribLoc = glGetAttribLocation(programID, "color");
    texCoordAttribLoc = glGetAttribLocation(programID, "texCoord");
    const size_t stride = sizeof(Vertex2D);
    glEnableVertexAttribArray(colorAttribLoc);
    glVertexAttribPointer(colorAttribLoc, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (GLvoid*) 0);
    glEnableVertexAttribArray(texCoordAttribLoc);
    glVertexAttribPointer(texCoordAttribLoc, 2, GL_UNSIGNED_SHORT, GL_FALSE, stride, (GLvoid*) 4);
    glEnableVertexAttribArray(posAttribLoc);
    glVertexAttribPointer(posAttribLoc, 3, GL_UNSIGNED_SHORT, GL_FALSE, stride, (GLvoid*) 8);
}