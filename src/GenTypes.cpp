#include "GenTypes.h"

using namespace std;
using namespace glm;

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

Pos2 Pos2::operator*(const float &rval)
{
    return Pos2(x * rval, y * rval);
}

void Pos2::operator+=(const Pos2 &rval)
{
    x += rval.x;
    y += rval.y;
}

Pos2 Pos2::operator+(const Pos2 &rval)
{
    Pos2 rv(x + rval.x, y + rval.y);
    return rv;
}

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
    r = MIN(r, 255);
    g = MIN(g, 255);
    b = MIN(b, 255);
    a = MIN(a, 255);
    r = MAX(r, 0);
    g = MAX(g, 0);
    b = MAX(b, 0);
    a = MAX(a, 0);
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

void Color4::operator*=(const float rval)
{
    int rr = r * rval;
    int gg = g * rval;
    int bb = b * rval;
    rr = MIN(rr, 255);
    gg = MIN(gg, 255);
    bb = MIN(bb, 255);
    rr = MAX(rr, 0);
    gg = MAX(gg, 0);
    bb = MAX(bb, 0);
    r = rr;
    g = gg;
    b = bb;
}

ostream& operator<<(ostream& os, const Rectangle& r)
{
    os << "(" << r.x << ", " << r.y << "), " << r.w << "x" << r.h;
    return os;
}

ostream& operator<<(ostream& os, const vec3& v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

ostream& operator<<(ostream& os, const vec4& v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}

ostream& operator<<(ostream& os, const mat4& mat)
{
    for(int i = 0; i < 4; i++)
    {
        os << "[";
        for(int j = 0; j < 4; j++)
        {
            os << mat[i][j];
            if(j != 3)
                os << " ";
        }
        os << "]" << endl;
    }
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

bool isPointInRect(Rectangle& rect, Pos2 pos)
{
    if(pos.x >= rect.x && pos.x < rect.x + rect.w && pos.y >= rect.y && pos.y < rect.y + rect.h)
        return true;
    return false;
}
