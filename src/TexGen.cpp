#include "TexGen.h"

using namespace std;
using namespace TexGen;

Color::Color()
{
    r = 255;
    g = 255;
    b = 255;
    a = 255;
}

Color::Color(byte rr, byte gg, byte bb, byte aa)
{
    r = rr;
    g = gg;
    b = bb;
    a = aa;
}

Generator::Generator(int x, int y) : buf(x, y) {}

bool TexGen::operator==(const TexGen::Color& c1, const TexGen::Color& c2)
{
    if(c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a)
        return true;
    return false;
}

Buffer::Buffer(int length, int width)
{
    len = length;
    wid = width;
    data = new Color[len * wid];
    fill({0, 0, 0, 255});
}

Buffer::Buffer(Buffer& buf)
{
    len = buf.len;
    wid = buf.wid;
    data = new Color[len * wid];
    memcpy(data, buf.data, sizeof(Color) * len * wid);
}

Buffer::~Buffer()
{
    delete[] data;
}

Color Buffer::get(int x, int y)
{
    if(validPx(x, y))
    {
        return data[x + y * wid];
    }
    else
        return {255, 255, 255, 255};
}

void Buffer::set(int x, int y, Color c)
{
    if(validPx(x, y))
        data[x + y * wid] = {c.r, c.g, c.b, c.a};
}

bool Buffer::validPx(int x, int y)
{
    if(x < 0 || x >= wid || y < 0 || y >= len)
        return false;
    return true;
}

void Buffer::smooth(int iters)
{
    for(int i = 0; i < iters; i++)
    {
        Buffer cpy = *this;
        for(int i = 0; i < len; i++)
        {
            for(int j = 0; j < wid; j++)
            {
                set(i, j, cpy.getAvg(i, j));
            }
        }
    }
}

Color Buffer::calcColor(TexGen::Color avg, int size, double rough)
{
    int range = rough * size;
    if(range == 0)
        return avg;
    int rn = rand() % range;
    int r = avg.r + rn - range / 2;
    int g = avg.g + rn - range / 2;
    int b = avg.b + rn - range / 2;
    int a = avg.a + rn - range / 2;
    r = max(r, 0);
    r = min(r, 255);
    g = max(g, 0);
    g = min(g, 255);
    b = max(b, 0);
    b = min(b, 255);
    a = max(a, 0);
    a = min(a, 255);
    return Color(r, g, b, a);
}

Color Buffer::getAvg(int x, int y)
{
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;
    int num = 0;
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            if(validPx(x + i, y + j))
            {
                Color c = get(x + i, y + j);
                r += c.r;
                g += c.g;
                b += c.b;
                a += c.a;
                num++;
            }
        }
    }
    if(num == 0)
        return {0, 0, 0, 255};
    return Color(r / num, g / num, b / num, a / num);
}

Color Buffer::getMode(int x, int y)
{
    vector<Color> colors;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            colors.push_back(get(x + i - 1, y + j - 1));
        }
    }
    int occur[9];
    memset(occur, 0, sizeof(int) * 9);
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            if(i == j)
                continue;
            if(colors[i] == colors[j])
            {
                occur[i + j * 3]++;
            }
        }
    }
    int best = 0;
    for(int i = 0; i < 9; i++)
    {
        if(occur[i] > occur[best])
            best = i;
    }
    return colors[best];
}

void Buffer::makeOpaque()
{
    for(int i = 0; i < len; i++)
    {
        for(int j = 0; j < wid; j++)
        {
            Color c = get(i, j);
            set(i, j, {c.r, c.g, c.b, 255});
        }
    }
}

void Generator::copyToAtlas(int texID)
{
    Atlas::sendImage((byte*) buf.data, texID);
}

void Buffer::fill(Color c)
{
    for(int i = 0; i < len; i++)
    {
        for(int j = 0; j < wid; j++)
        {
            set(i, j, c);
        }
    }
}

void Buffer::fillNoAlpha(Color c)
{
    for(int i = 0; i < len; i++)
    {
        for(int j = 0; j < wid; j++)
        {
            byte alpha = get(i, j).a;
            set(i, j, {c.r, c.g, c.b, alpha});
        }
    }
}

Cloud::Cloud(int x, int y) : Generator(x, y) {}

void Cloud::generate()
{
    Heightmap h(buf.len, buf.wid);
    h.diamondSquare(2, 5000, 5000, false);
    for(int i = 0; i < buf.len; i++)
    {
        for(int j = 0; j < buf.wid; j++)
        {
            int val = h.get(i, j) * 255.0 / SHRT_MAX;
            buf.set(i, j, Color(255, 255, 255, val));
        }
    }
}

Plains::Plains(int x, int y) : Generator(x, y) {}

void Plains::generate()
{
    for(int i = 0; i < buf.len; i++)
    {
        for(int j = 0; j < buf.wid; j++)
        {
            int choice = RandomUtils::gen() % 3;
            switch(choice)
            {
                case 0:
                    buf.set(i, j, {0x20, 0x40, 0x20, 0xFF});
                    break;
                case 1:
                    buf.set(i, j, {0x30, 0x50, 0x30, 0xFF});
                    break;
                case 2:
                    buf.set(i, j, {0x40, 0x60, 0x40, 0xFF});
            }
        }
    }
    buf.smooth(1);
}

Forest::Forest(int x, int y) : Generator(x, y) {}

void Forest::generate()
{
    for(int i = 0; i < buf.len; i++)
    {
        for(int j = 0; j < buf.wid; j++)
        {
            int choice = rand() % 3;
            // 0 = green, 1 = brown, 2 = grey
            switch(choice)
            {
                case 0:
                    buf.set(i, j, {0x20, 0x90, 0x20, 0xFF});
                    break;
                case 1:
                    buf.set(i, j, {0x30, 0x50, 0x30, 0xFF});
                    break;
                case 2:
                    buf.set(i, j, {0x40, 0x50, 0x40, 0xFF});
            }
        }
    }
    buf.smooth(10);
}