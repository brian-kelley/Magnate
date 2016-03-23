#include "Shaders.h"

using namespace std;

Shaders::Shaders()
{
    const char* vshader =
    "#version 120\n"
    "attribute vec3 vertex;\n"
    "attribute vec4 color;\n"
    "attribute vec2 texCoord;\n"
    "attribute vec3 normal;\n"
    "varying vec2 fragTexCoord;\n"
    "varying vec4 fragBaseColor;\n"
    "varying vec4 fragNormal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "uniform int useNormals;\n"
    "uniform vec3 sun;\n"
    "uniform sampler2D sampler;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = proj * view * model * vec4(vertex, 1.0);\n"
    "    fragTexCoord = texCoord;\n"
    "    fragBaseColor = color;\n"
    "    fragNormal = vec4(normal, 1.0);\n"
    "}\n";
    
    const char* fshader =
    "#version 120\n"
    "uniform sampler2D sampler;\n"
    "uniform vec3 sun;\n"
    "uniform int useNormals;\n"
    "varying vec2 fragTexCoord;\n"
    "varying vec4 fragBaseColor;\n"
    "varying vec4 fragNormal;\n"
    "void main()\n"
    "{\n"
    "    if(fragTexCoord.x == -1)\n"
    "    {\n"
    "        gl_FragColor = fragBaseColor;\n"
    "        if(useNormals != 0)\n"
    "        {\n"
    "            vec4 diffuseColor = gl_FragColor * abs(dot(sun, vec3(fragNormal)));\n"
    "            gl_FragColor = 0.8 * gl_FragColor + 0.2 * diffuseColor;\n"
    "        }\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        vec2 normalTexCoord = fragTexCoord / 2048.0;\n"
    "        vec4 texelColor = texture2D(sampler, normalTexCoord);\n"
    "        gl_FragColor.x = texelColor.x * fragBaseColor.x;\n"
    "        gl_FragColor.y = texelColor.y * fragBaseColor.y;\n"
    "        gl_FragColor.z = texelColor.z * fragBaseColor.z;\n"
    "        gl_FragColor.w = texelColor.w * fragBaseColor.w;\n"
    "        if(useNormals != 0)\n"
    "        {\n"
    "           vec4 diffuseColor = gl_FragColor * abs(dot(sun, vec3(fragNormal)));\n"
    "           diffuseColor.w = 1;\n"
    "           gl_FragColor = 0.7 * gl_FragColor + 0.3 * diffuseColor;\n"
    "        }\n"
    "    }\n"
    "}\n";
    programID = glCreateProgram();
    vshadeID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshadeID, 1, &vshader, NULL);
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
    glShaderSource(fshadeID, 1, &fshader, NULL);
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
    sunLoc = glGetUniformLocation(programID, "sun");
    //set and forget sunlight direction for now
    float sunData[] = {0.3, 0.9055, 0.3};
    glUniform3fv(sunLoc, 1, sunData);
    GLERR;
    glm::mat4 idModel;
    glUniformMatrix4fv(modelLoc, 1, false, glm::value_ptr(idModel));
    GLERR;
    VBO::loadAttribLocs(programID);
}

Shaders::~Shaders()
{
    glDetachShader(programID, vshadeID);
    glDetachShader(programID, fshadeID);
    glDeleteShader(vshadeID);
    glDeleteShader(fshadeID);
    glDeleteProgram(programID);
}

GLuint Shaders::getViewMatLoc()
{
    return viewLoc;
}

GLuint Shaders::getProjMatLoc()
{
    return projLoc;
}

GLuint Shaders::getProgramID()
{
    return programID;
}
