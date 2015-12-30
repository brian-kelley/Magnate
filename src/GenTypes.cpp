#include "GenTypes.h"

using namespace std;

Rectangle::Rectangle() {}
Rectangle::Rectangle(int x, int y, int w, int h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

ostream& operator<<(ostream& os, Rectangle& r)
{
    os << "(" << r.x << "," << r.y << "), " << r.w << "x" << r.h;
    return os;
}