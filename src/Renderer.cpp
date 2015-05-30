#include "Renderer.h"

using namespace std;

vector<vertex> Renderer::quadVertices;
vector<vertex> Renderer::lineVertices;
bool Renderer::textureOn = true;
int Renderer::numQuadVertices = 0;
int Renderer::numLineVertices = 0;
GLuint Renderer::vboID;
GLuint Renderer::programID;
GLuint Renderer::vshadeID;
GLuint Renderer::fshadeID;
GLuint Renderer::positionAttrib;
GLuint Renderer::colorAttrib;
GLuint Renderer::texcoordAttrib;
int Renderer::preloadSize = 100;
int Renderer::vboSize;
vertex Renderer::stateVertex;

void Renderer::initAll()
{
    initShaders();
    initVBO();
}

void Renderer::initVBO()
{
    quadVertices.reserve(preloadSize);
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * preloadSize, NULL, GL_STREAM_DRAW);
    vboSize = sizeof(vertex) * preloadSize;
    //To do this stuff, *Attrib variables must be set by now
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) 0);
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) (2 * sizeof(float)));
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) (6 * sizeof(float)));
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
    //cout << glGetString(GL_SHADING_LANGUAGE_VERSION);
    vector<string> code = {
    "#version 120",
    "attribute vec2 vertex;",
    "attribute vec4 color;",
    "attribute vec2 texcoord;",
    "varying vec2 ftexcoord;",
    "varying vec4 fcolor;",
    "void main()",
    "{",
    "    gl_Position = gl_ModelViewProjectionMatrix * vec4(vec3(vertex, 0.0), 1.0);",
    "    ftexcoord = texcoord;",
    "    fcolor = color;",
    "}"};
    string vertexShadeCode = "";
    for(auto line : code)
    {
        vertexShadeCode += line + "\n";
    }
    code = {
    "#version 120",
    "uniform sampler2D sampler;",
    "varying vec2 ftexcoord;",
    "varying vec4 fcolor;",
    "void main()",
    "{",
    "    if(ftexcoord.x < 0 || ftexcoord.y < 0)",
    "    {",
    "        gl_FragColor = fcolor;",
    "    }",
    "    else",
    "    {",
    "        vec4 texelColor = texture2D(sampler, ftexcoord);",
    "        //Scale texel colors by vertex color",
    "        gl_FragColor.x = texelColor.x * fcolor.x;",
    "        gl_FragColor.y = texelColor.y * fcolor.y;",
    "        gl_FragColor.z = texelColor.z * fcolor.z;",
    "        gl_FragColor.w = texelColor.w * fcolor.w;",
    "    }",
    "}"};
    string fragmentShadeCode = "";
    for(auto line : code)
    {
        fragmentShadeCode += line + "\n";
    }
    const char* vcstr = vertexShadeCode.c_str();
    const char* fcstr = fragmentShadeCode.c_str();
    programID = glCreateProgram();
    vshadeID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshadeID, 1, &vcstr, NULL);
    cout << "Compiling vertex shader." << endl;
    glCompileShader(vshadeID);
    char error[1000];
    glGetShaderInfoLog(vshadeID, 1000, NULL, error);
    cout << error;
    fshadeID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshadeID, 1, &fcstr, NULL);
    cout << "Compiling fragment shader." << endl;
    glCompileShader(fshadeID);
    glGetShaderInfoLog(fshadeID, 1000, NULL, error);
    cout << error;
    glAttachShader(programID, vshadeID);
    glAttachShader(programID, fshadeID);
    glBindAttribLocation(programID, 0, "vertex");
    glBindAttribLocation(programID, 1, "color");
    glBindAttribLocation(programID, 2, "texcoord");
    positionAttrib = 0;
    colorAttrib = 1;
    texcoordAttrib = 2;
    glLinkProgram(programID);
    glUseProgram(programID);
    GLuint samplerLoc = glGetUniformLocation(programID, "sampler");
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(samplerLoc, 0);
    glEnableVertexAttribArray(positionAttrib);
    glEnableVertexAttribArray(colorAttrib);
    glEnableVertexAttribArray(texcoordAttrib);
}

void Renderer::startFrame()
{
    numQuadVertices = 0;
    numLineVertices = 0;
}

void Renderer::endFrame()
{
    int totalVertices = numQuadVertices + numLineVertices;
    if(sizeof(vertex) * (totalVertices) > vboSize)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * totalVertices, NULL, GL_STREAM_DRAW);
        vboSize = sizeof(vertex) * totalVertices;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex) * numQuadVertices, &quadVertices.front());
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex) * numQuadVertices, sizeof(vertex) * numLineVertices, &lineVertices.front());
    glDrawArrays(GL_QUADS, 0, numQuadVertices);
    glDrawArrays(GL_LINES, numQuadVertices, numLineVertices);
}

void Renderer::color3f(float r, float g, float b)
{
    stateVertex.r = r;
    stateVertex.g = g;
    stateVertex.b = b;
    stateVertex.a = 1.0f;
}

void Renderer::color4f(float r, float g, float b, float a)
{
    stateVertex.r = r;
    stateVertex.g = g;
    stateVertex.b = b;
    stateVertex.a = a;
}

void Renderer::vertex2i(int x, int y)
{
    stateVertex.x = x;
    stateVertex.y = y;
    //Just save vertex here, because color/texcoord will have already been set in stateVertex (emulating how OpenGL immediate mode would work)
    addQuadVertex();
}

void Renderer::lineVertex2i(int x, int y)
{
    stateVertex.x = x;
    stateVertex.y = y;
    addLineVertex();
}

void Renderer::texCoord2f(float u, float v)
{
    stateVertex.u = u;
    stateVertex.v = v;
    textureOn = true;
}

void Renderer::addQuadVertex()
{
    if(!textureOn)
    {
        //Shaders will know that these coords mean to not use texture2D for color
        stateVertex.u = -1.0f;
        stateVertex.v = -1.0f;
    }
    //else: texcoord must have already been set by program
    if(quadVertices.size() > numQuadVertices)
    {
        quadVertices[numQuadVertices] = stateVertex;
    }
    else
    {
        quadVertices.push_back(stateVertex);
    }
    numQuadVertices++;
}

void Renderer::addLineVertex()
{
    if(!textureOn)
    {
        //Shaders will know that these coords mean to not use texture2D for color
        stateVertex.u = -1.0f;
        stateVertex.v = -1.0f;
    }
    //else: texcoord must have already been set by program
    if(lineVertices.size() > numLineVertices)
    {
        lineVertices[numLineVertices] = stateVertex;
    }
    else
    {
        lineVertices.push_back(stateVertex);
    }
    numLineVertices++;
}

void Renderer::enableTexture()
{
    textureOn = true;
}

void Renderer::disableTexture()
{
    textureOn = false;
}