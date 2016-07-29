#ifndef __GENTYPES_H__
#define __GENTYPES_H__

#include <iostream>
#include <cstdint>
#include <iomanip>
#include "GlmHeaders.h"

using namespace std;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

using glm::vec2;
using glm::vec3;
using glm::vec4;

typedef void(*CallbackFunc)(void*, void*);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SWAP(a, b) {auto temp = a; a = b; b = temp;}

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
    Color4 color;       //4
    TexCoord texcoord;  //4
    Pos2 pos;           //4
}; //12 bytes total

struct Vertex3D
{
    Color4 color;       //4
    TexCoord texcoord;  //4
    vec3 pos;           //12
    vec3 norm;          //12
}; //32 bytes total

std::ostream& operator<<(std::ostream& os, const Rectangle& r);
std::ostream& operator<<(std::ostream& os, const glm::vec2& v);
std::ostream& operator<<(std::ostream& os, const glm::vec3& v);
std::ostream& operator<<(std::ostream& os, const glm::vec4& v);
std::ostream& operator<<(std::ostream& os, const glm::mat4& v);
u32 getColor32(u8 r, u8 g, u8 b, u8 a = 0xFF);

template<typename T>
void sort3(T in1, T in2, T in3, T& out1, T& out2, T& out3)
{
    out1 = in1;
    out2 = in2;
    out3 = in3;
    if(out1 > out2)
        SWAP(out1, out2);
    if(out2 > out3)
        SWAP(out2, out3);
    if(out1 > out2)
        SWAP(out1, out2);
}

#endif
