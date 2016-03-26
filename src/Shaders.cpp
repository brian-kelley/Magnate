#include "Shaders.h"

using namespace std;

Shaders::Shaders()
{
    vector<string> code = {
        "#version 120",
        "attribute vec3 vertex;",
        "attribute vec4 color;",
        "attribute vec2 texCoord;",
        "varying vec2 fragTexCoord;",
        "varying vec4 fragBaseColor;",
        "uniform mat4 model;",
        "uniform mat4 view;",
        "uniform mat4 proj;",
        "uniform sampler2D sampler;",
        "void main()",
        "{",
        "    gl_Position = proj * view * model * vec4(vertex, 1.0);",
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
    modelLoc = glGetUniformLocation(programID, "model");
    viewLoc = glGetUniformLocation(programID, "view");
    projLoc = glGetUniformLocation(programID, "proj");
    GLERR
    VBO::loadAttribLocs(programID);
    GLERR
}

Shaders::~Shaders()
{
    glDetachShader(programID, vshadeID);
    glDetachShader(programID, fshadeID);
    glDeleteShader(vshadeID);
    glDeleteShader(fshadeID);
    glDeleteProgram(programID);
}
