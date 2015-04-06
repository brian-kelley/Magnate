#ifndef __VBOMANAGER__INCLUDED__
#define __VBOMANAGER__INCLUDED__
#ifdef _WIN32
#include "GL/gl.h"
#elif __APPLE__
#include "gl.h"
#endif

#include <iostream>
#include <vector>
#include <string>
#include "VBOMember.h"

namespace VBOManager
{
    std::vector<VBOMember> members;
    GLuint vboID;
    int vboSize;    //sum of actual sizes of members, used w/glBufferData
    void initVBO();
    void allocVRAM();   //glBufferData with NULL to alloc space in GPU
    void drawMembers(); //glDrawArrays for each member
    std::string VS_STR = "\
    Put shader code here\
    ";
    std::string FS_STR = "\
    Put moar shader code here\
    ";
}

#endif