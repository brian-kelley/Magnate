#include "Heightmap.h"

using namespace std;

Heightmap::Heightmap()
{
    buf = NULL;
    w = 0;
    h = 0;
}

Heightmap::Heightmap(int x, int y)
{
    //rows together
    //(x, y) is at offset x + y * h
    buf = new short[x * y];
    w = x;
    h = y;
    memset(buf, 0, x * y * sizeof(short));
}

Heightmap::Heightmap(const Heightmap& toCopy)
{
    w = toCopy.getW();
    h = toCopy.getH();
    buf = new short[w * h];
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            set(toCopy.get(i, j), i, j);
        }
    }
}

Heightmap::~Heightmap()
{
    delete[] buf;
}

void Heightmap::diamondSquare(double roughness, short cornerStart, short centerStart, bool isZeroMin)
{
    floorZero = isZeroMin;
    int newsz = 1;
    while(newsz + 1 < w && newsz + 1 < h)
        newsz *= 2;
    int size = newsz;
    newsz++;
    bool useNewBuf = newsz != w || newsz != h;
    Heightmap* buf;
    if(useNewBuf)
        buf = new Heightmap(newsz, newsz);
    else
        buf = this;
    buf->set(0, 0, cornerStart);
    buf->set(0, newsz - 1, cornerStart);
    buf->set(newsz - 1, 0, cornerStart);
    buf->set(newsz - 1, newsz - 1, cornerStart);
    buf->set(newsz / 2, newsz / 2, centerStart);
    while(size > 1)
    {
        for(int i = size / 2; i < newsz; i += size)
        {
            for(int j = size / 2; j < newsz; j += size)
            {
                buf->fillSquare(i, j, size, roughness);
            }
        }
        for(int i = 0; i < newsz; i += size / 2)
        {
            for(int j = 0; j < newsz; j += size / 2)
            {
                if((i + j) % size)
                {
                    buf->fillDiamond(i, j, size, roughness);
                }
            }
        }
        size /= 2;
    }
    if(useNewBuf)
    {
        //Now copy newbuf into this if they are different
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < h; j++)
            {
                set(buf->get(i, j), i, j);
            }
        }
        delete buf;
    }
}

void Heightmap::fillSquare(int x, int y, int sz, double rough)
{
    int sum = 0;
    int num = 0;
    int xi = x - sz / 2;
    int yi = y - sz / 2;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    xi = x + sz / 2;
    yi = y - sz / 2;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    xi = x + sz / 2;
    yi = y + sz / 2;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    xi = x - sz / 2;
    yi = y + sz / 2;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    if(num == 0)    //Don't make any changes
        return;
    short newVal = calcVal(sum / num, sz, rough);
    set(newVal, x, y);
}

void Heightmap::fillDiamond(int x, int y, int sz, double rough)
{
    int sum = 0;
    int num = 0;
    int xi = x - sz / 2;
    int yi = y;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    xi = x;
    yi = y - sz / 2;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    xi = x + sz / 2;
    yi = y;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    xi = x;
    yi = y + sz / 2;
    if(validPoint(xi, yi))
    {
        sum += get(xi, yi);
        num++;
    }
    if(num == 0)
        return;
    short newVal = calcVal(sum / num, sz, rough);
    set(newVal, x, y);
}

bool Heightmap::validPoint(int x, int y) const
{
    if(x < 0 || x >= w || y < 0 || y >= h)
        return false;
    return true;
}

short Heightmap::calcVal(int avg, int size, double roughness)
{
    int range = roughness * size;
    if(range == 0)
        return avg;
    int rn = RandomUtils::gen() % range;
    int newVal = avg + rn - range / 2;
    if(floorZero)
        newVal = max(newVal, 0);
    else
    {
        newVal = max(newVal, SHRT_MIN);
        newVal = min(newVal, SHRT_MAX);
    }
    return newVal;
}

short Heightmap::get(int x, int y) const
{
    if(validPoint(x, y))
        return buf[x + y * w];
    else
        return 0;
}

short Heightmap::get(Pos2 loc) const
{
    if(validPoint(loc.x, loc.y))
        return buf[loc.x + loc.y * w];
    else
        return 0;
}

void Heightmap::set(short val, int x, int y)
{
    if(validPoint(x, y))
        buf[x + y * w] = val;
}

void Heightmap::set(short val, Pos2 loc)
{
    if(validPoint(loc.x, loc.y))
        buf[loc.x + loc.y * w] = val;
}

void Heightmap::set(short val)
{
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            set(val, i, j);
        }
    }
}

void Heightmap::add(const Heightmap &rval)
{
    DBASSERT(w == rval.w && h == rval.h);
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            add(rval.get(i, j), i, j);
        }
    }
}

void Heightmap::add(short val, int x, int y)
{
    if(validPoint(x, y))
        buf[x + y * w] += val;
}

void Heightmap::add(short val, Pos2 loc)
{
    if(validPoint(loc.x, loc.y))
        buf[loc.x + loc.y * w] += val;
}

void Heightmap::operator+=(Heightmap &hm)
{
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            add(hm.get(i, j), i, j);
        }
    }
}

void Heightmap::operator-=(Heightmap &hm)
{
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            add(-hm.get(i, j), i, j);
        }
    }
}

Heightmap Heightmap::operator*(Heightmap &hm)
{
    Heightmap newHM(w, h);
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            newHM.set(get(i, j) * hm.get(i, j) / 1000.0, i, j);
        }
    }
    return newHM;
}

void Heightmap::operator*=(Heightmap& hm)
{
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            set(get(i, j) * hm.get(i, j) / 1000.0, i, j);
        }
    }
}

Heightmap& Heightmap::operator=(Heightmap &hm)
{
    if(buf)
    {
        delete buf;
        buf = NULL;
    }
    w = hm.getW();
    h = hm.getH();
    buf = new short[w * h];
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            set(hm.get(i, j), i, j);
        }
    }
    return *this;
}

int Heightmap::getW() const
{
    return w;
}

int Heightmap::getH() const
{
    return h;
}

int Heightmap::getSteepness(int x, int y)
{
    int sx = abs(get(x, y) - get(x - 1, y)) + abs(get(x + 1, y) - get(x, y));
    int sy = abs(get(x, y) - get(x, y - 1)) + abs(get(x, y + 1) - get(x, y));
    return sqrt(sx * sx + sy * sy);
}

int Heightmap::getSteepness(Pos2 loc)
{
    return getSteepness(loc.x, loc.y);
}

void Heightmap::normalize(int val)
{
    int minh = SHRT_MAX;
    int maxh = SHRT_MIN;
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            if(get(i, j) > maxh)
                maxh = get(i, j);
            if(get(i, j) < minh)
                minh = get(i, j);
        }
    }
    float scl = float(val) / (maxh - minh);
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            set((get(i, j) - minh) * scl, i, j);
        }
    }
}

glm::vec2 Heightmap::grad(int x, int y)
{
    int sx = (get(x + 1, y) - get(x, y) + get(x + 1, y + 1) - get(x, y + 1)) / 2;
    int sy = (get(x, y + 1) - get(x, y) + get(x + 1, y + 1) - get(x + 1, y)) / 2;
    if(sx > CHAR_MAX)
        sx = CHAR_MAX;
    if(sx < CHAR_MIN)
        sx = CHAR_MIN;
    if(sy > CHAR_MAX)
        sy = CHAR_MAX;
    if(sy < CHAR_MIN)
        sy = CHAR_MIN;
    return glm::vec2(sx, sy);
}

glm::vec2 Heightmap::grad(Pos2 loc)
{
    return grad(loc.x, loc.y);
}

void Heightmap::setSize(int x, int y)
{
    DBASSERT(x > 0);
    DBASSERT(y > 0);
    delete[] buf;
    buf = new short[x * y];
    w = x;
    h = y;
}


void Heightmap::smooth(int iters)
{
    Heightmap copy(w, h);
    for(int it = 0; it < iters; it++)
    {
        copy.set(0);
        copy.add(*this);
        for(int i = 1; i < w - 1; i++)
        {
            for(int j = 1; j < h - 1; j++)
            {
                int sum = 0;
                sum += copy.get(i, j);
                sum += copy.get(i + 1, j);
                sum += copy.get(i - 1, j);
                sum += copy.get(i, j + 1);
                sum += copy.get(i, j - 1);
                set(sum / 5, i, j);
            }
        }
    }
}
