#ifndef __GENTYPES_H__
#define __GENTYPES_H__

#include <iostream>
#include <cstdint>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

struct Rectangle
{
    Rectangle();
    Rectangle(int x, int y, int w, int h);
    int x;
    int y;
    int w;
    int h;
};

std::ostream& operator<<(std::ostream& os, Rectangle& r);

#endif