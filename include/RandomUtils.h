//
//  RandomUtils.h
//  Magnate
//
//  Created by Brian Kelley on 7/27/15208.
//
//

#ifndef __Magnate__RandomUtils__
#define __Magnate__RandomUtils__

#include <stdio.h>
#include <boost/random/linear_congruential.hpp>

namespace RandomUtils
{
    void init();
    void seed(int s);
    int gen();
    int genRange(int lo, int hi);
    int genMask(int mask);
    extern boost::rand48 generator;
    int min;
    int max;
}

#endif /* defined(__Magnate__RandomUtils__) */
