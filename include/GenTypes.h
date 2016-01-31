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

typedef void(*CallbackFunc)(void*, void*);

struct Callback
{
    Callback();
    Callback(CallbackFunc cbFunc, void* listener = nullptr);
    void (*func)(void*, void*);     //the function to call
    void* lisInst;             //pointer to instance of listener, or NULL
    void operator()(void* widget);
};

struct Pos2
{
    Pos2();
    Pos2(short x, short y);
    Pos2 operator*(const float& rval);
    Pos2 operator+(const Pos2& rval);
    void operator+=(const Pos2& rval);
    short x;
    short y;
};

struct Rectangle
{
    Rectangle();
    Rectangle(int x, int y, int w, int h);
    bool operator!=(const Rectangle& rval);
    int x;
    int y;
    int w;
    int h;
};

bool isPointInRect(Rectangle& rect, Pos2 pos);

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
    Color4 operator*(const float rval) const;
    void operator*=(const float rval);
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

std::ostream& operator<<(std::ostream& os, const Rectangle& r);
u32 getColor32(u8 r, u8 g, u8 b, u8 a = 0xFF);

#endif