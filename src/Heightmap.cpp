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
        //Now copy newbuf's pixels into thisbuf if a new buffer was used
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
        newVal = newVal < 0 ? 0 : newVal;
    else
        newVal = newVal < SHRT_MIN ? SHRT_MIN : newVal;
    newVal = newVal > SHRT_MAX ? SHRT_MAX : newVal;
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

VecField Heightmap::gradient()
{
    VecField vf(w, h);
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            //get average slope in x and y
            int ddx = (get(i + 1, j) - get(i - 1, j)) / 2;
            int ddy = (get(i, j + 1) - get(i, j - 1)) / 2;
            //clamp to char range (real-world usage should rarely need it)
            ddx = ddx < CHAR_MIN ? CHAR_MIN : ddx;
            ddx = ddx > CHAR_MAX ? CHAR_MAX : ddx;
            ddy = ddy < CHAR_MIN ? CHAR_MIN : ddy;
            ddy = ddy > CHAR_MAX ? CHAR_MAX : ddy;
            //put SmallVec in field
            vf.set(SmallVec(ddx, ddy), i, j);
        }
    }
    return vf;
}

int Heightmap::getW() const
{
    return w;
}

int Heightmap::getH() const
{
    return h;
}