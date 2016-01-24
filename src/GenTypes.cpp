#include "GenTypes.h"

using namespace std;

Callback::Callback()
{
    func = nullptr;
    lisInst = nullptr;
}

Callback::Callback(CallbackFunc cbFunc, void* listener)
{
    func = cbFunc;
    lisInst = listener;
}

void Callback::operator()(void *widget)
{
    if(func)
        func(lisInst, widget);
}

Pos2::Pos2() : x(0), y(0) {}
Pos2::Pos2(short xx, short yy) : x(xx), y(yy) {}

Rectangle::Rectangle() {}
Rectangle::Rectangle(int x, int y, int w, int h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

bool Rectangle::operator!=(const Rectangle &rval)
{
    return x != rval.x || y != rval.y || w != rval.w || h != rval.h;
}

TexCoord::TexCoord()
{
    u = -1;
    v = -1;
}

TexCoord::TexCoord(int u, int v)
{
    this->u = u;
    this->v = v;
}

Color4::Color4()
{
    r = 255;
    g = 255;
    b = 255;
    a = 255;
}

Color4::Color4(int r, int g, int b, int a)
{
    r = min(r, 255);
    g = min(g, 255);
    b = min(b, 255);
    a = min(a, 255);
    r = max(r, 0);
    g = max(g, 0);
    b = max(b, 0);
    a = max(a, 0);
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Color4::operator=(const Color4 &rval)
{
    r = rval.r;
    g = rval.g;
    b = rval.b;
    a = rval.a;
}

Color4 Color4::operator*(const float rval) const
{
    return Color4(r * rval, g * rval, b * rval, a);
}

ostream& operator<<(ostream& os, const Rectangle& r)
{
    os << "(" << r.x << "," << r.y << "), " << r.w << "x" << r.h;
    return os;
}

u32 getColor32(u8 r, u8 g, u8 b, u8 a)
{
    u32 rr = r;
    u32 gg = g;
    u32 bb = b;
    u32 aa = a;
#if defined(__APPLE__)
    //BGRA
    return aa << 24 | rr << 16 | gg << 8 | bb;
#elif defined(_WIN32)
    //RGBA
    return rr << 24 | gg << 16 | bb << 8 | aa;
#endif
}