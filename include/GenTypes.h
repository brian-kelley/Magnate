#ifndef __GENTYPES_H__
#define __GENTYPES_H__

#include <iostream>
#include <cstdint>
#include "GlmHeaders.h"

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef glm::vec3 Pos3;

struct Pos2
{
    Pos2();
    Pos2(short x, short y);
    short x;
    short y;
};

struct Rectangle
{
    Rectangle();
    Rectangle(int x, int y, int w, int h);
    int x;
    int y;
    int w;
    int h;
};

struct TexCoord
{
    TexCoord();
    TexCoord(int u, int v);
    short u;
    short v;
};

struct Color4
{
    Color4();
    Color4(int r, int g, int b, int a);
    void operator=(const Color4& rval);
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

struct Vertex2D
{
    Color4 color;
    TexCoord texcoord;
    Pos2 pos;
}; //12 bytes total

struct Vertex3D
{
    Color4 color;
    TexCoord texcoord;
    Pos3 pos;
}; //20 bytes total

std::ostream& operator<<(std::ostream& os, Rectangle& r);
u32 getColor32(u8 r, u8 g, u8 b, u8 a = 0xFF);

#endif