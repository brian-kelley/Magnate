#include "RandomUtils.h"

using namespace boost;

rand48 RandomUtils::generator;

void RandomUtils::init()
{
    generator = rand48();
}

void RandomUtils::seed(int s)
{
    generator.seed(s);
}

int RandomUtils::gen()
{
    return generator();
}

int RandomUtils::genRange(int lo, int hi)
{
    return lo + generator() % (hi - lo);
}

int RandomUtils::genMask(int mask)
{
    return generator() & mask;
}

float RandomUtils::genFloat()
{
    const int num = 0x10000000; //# of discrete floats to be returned - todo: improve
    return float(generator() % num) / num;
}