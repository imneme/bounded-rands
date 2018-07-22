/*
 * A C++ implementation methods and benchmarks for random numbers in a range
 * (32-bit version)
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Melissa E. O'Neill
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <random>
#include "pcg_random.hpp"
#include "timer.hpp"

#ifdef RNG_INCLUDE
    #include RNG_INCLUDE
#endif

#ifndef RNG_TYPE
    #define RNG_TYPE std::mt19937
#endif

using rng_t = RNG_TYPE;

#if USE_STD

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    std::uniform_int_distribution<uint32_t> dist(0, range-1);

    return dist(rng);
}

#elif USE_BIASED_FP_MULT_LDEXP

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    double zeroone = ldexp(rng(), -32);
    return range * zeroone;
}

#elif USE_BIASED_FP_MULT_SCALE

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    double zeroone = 0x1.0p-32 * rng();
    return range * zeroone;
}

#elif USE_BIASED_MOD

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    return rng() % range;
}

#elif USE_DEBIASED_DIV

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t divisor = ((-range) / range) + 1;
    if (divisor == 0)
	return 0;
    for (;;) {
        uint32_t val = rng() / divisor;
        if (val < range)
            return val;
    }
}

#elif USE_DEBIASED_MODx2

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t t = (-range) % range;
    for (;;) {
        uint32_t r = rng();
        if (r >= t)
            return r % range;
    }
}

#elif USE_DEBIASED_MODx2_MOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t t = -range;
    if (t >= range) {
	t -= range;
	if (t >= range) 
	    t %= range;
    }
    for (;;) {
        uint32_t r = rng();
        if (r >= t)
            return r % range;
    }
}

#elif USE_DEBIASED_MODx2_TOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t r = rng();
    if (r < range) {
	uint32_t t = (-range) % range;
	while (r < t)
	    r = rng();
    }
    return r % range;
}

#elif USE_DEBIASED_MODx2_TOPT_BOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t r = rng();
    if (range >= 1u<<31) {
	while(r >= range)
	    r = rng();
	return r;
    }
    if (r < range) {
	uint32_t t = (-range) % range;
	while (r < t)
	    r = rng();
    }
    return r % range;
}

#elif USE_DEBIASED_MODx2_TOPT_MOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t r = rng();
    if (r < range) {
	uint32_t t = -range;
	if (t >= range) {
	    t -= range;
	    if (t >= range) 
		t %= range;
	}
	while (r < t)
	    r = rng();
    }
    return r % range;
}

#elif USE_DEBIASED_MODx2_TOPT_MOPTx2

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t r = rng();
    if (r < range) {
	uint32_t t = -range;
	if (t >= range) {
	    t -= range;
	    if (t >= range) 
		t %= range;
	}
	while (r < t)
	    r = rng();
    }
    if (r >= range) {
	r -= range;
	if (r >= range)
	    r %= range;
    }
    return r;
}

#elif USE_DEBIASED_MODx1

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x, r;
    do {
	x = rng();
	r = x % range;
    } while (x - r > uint32_t(-range));
    return r;
}

#elif USE_DEBIASED_MODx1_BOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x, r;
    if (range >= 1u<<31) {
	do {
	    r = rng();
	} while (r >= range);
	return r;
    }
    do {
	x = rng();
	r = x % range;
    } while (x - r > uint32_t(-range));
    return r;
}

#elif USE_DEBIASED_MODx1_MOPT
      
static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x, r;
    do {
	x = rng();
	r = x;
	if (r >= range) {
	    r -= range;
	    if (r >= range)
		r %= range;
	}
    } while (x - r > uint32_t(-range));
    return r;
}

#elif USE_BIASED_INT_MULT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x = rng();
    uint64_t m = uint64_t(x) * uint64_t(range);
    return m >> 32;
}

#elif USE_DEBIASED_INT_MULT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t t = (-range) % range;
    uint32_t l;
    uint64_t m;
    do {
	uint32_t x = rng();
	m = uint64_t(x) * uint64_t(range);
	l = uint32_t(m);
    } while (l < t);
    return m >> 32;
}


#elif USE_DEBIASED_INT_MULT_TOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x = rng();
    uint64_t m = uint64_t(x) * uint64_t(range);
    uint32_t l = uint32_t(m);
    if (l < range) {
	uint32_t t = (-range) % range;
	while (l < t) {
	    x = rng();
	    m = uint64_t(x) * uint64_t(range);
	    l = uint32_t(m);
	}
    }
    return m >> 32;
}

#elif USE_DEBIASED_INT_MULT_TOPT_BOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x = rng();
    if (range >= 1ul<<31) {
	while(x >= range)
	    x = rng();
	return x;
    }
    uint64_t m = uint64_t(x) * uint64_t(range);
    uint32_t l = uint32_t(m);
    if (l < range) {
	uint32_t t = (-range) % range;
	while (l < t) {
	    x = rng();
	    m = uint64_t(x) * uint64_t(range);
	    l = uint32_t(m);
	}
    }
    return m >> 32;
}

#elif USE_DEBIASED_INT_MULT_TOPT_MOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x = rng();
    uint64_t m = uint64_t(x) * uint64_t(range);
    uint32_t l = uint32_t(m);
    if (l < range) {
	uint32_t t = -range;
	if (t >= range) {
	    t -= range;
	    if (t >= range) 
		t %= range;
	}
	while (l < t) {
	    x = rng();
	    m = uint64_t(x) * uint64_t(range);
	    l = uint32_t(m);
	}
    }
    return m >> 32;
}

#elif USE_DEBIASED_INT_MULT_TOPT_MOPT_BOPT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t x = rng();
    if (range >= 1u<<31) {
	while(x >= range)
	    x = rng();
	return x;
    }
    uint64_t m = uint64_t(x) * uint64_t(range);
    uint32_t l = uint32_t(m);
    if (l < range) {
	uint32_t t = -range;
	t -= range;
	if (t >= range) 
	    t %= range;
	while (l < t) {
	    x = rng();
	    m = uint64_t(x) * uint64_t(range);
	    l = uint32_t(m);
	}
    }
    return m >> 32;
}

#elif USE_BITMASK

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    uint32_t mask = ~uint32_t(0);
    --range;
    mask >>= __builtin_clz(range|1);
    uint32_t x;
    do {
	x = rng() & mask;
    } while (x > range);
    return x;
}

#elif USE_BITMASK_ALT

static uint32_t bounded_rand(rng_t& rng, uint32_t range) {
    --range;
    unsigned int zeros = __builtin_clz(range|1);
    uint32_t mask = (~uint32_t(0)) >> zeros;
    for (;;) {
	uint32_t r = rng();
	uint32_t v = r & mask;
	if (v <= range)
	    return v;
	unsigned int shift = 16;
	while (zeros >= shift) {
	    r >>= shift;
	    v = r & mask;
	    if (v <= range)
		return v;
	    shift = 32 - (32 - shift)/2;
	}
    }
}

#endif

int main(int argc, char* argv[])
{
    uint64_t sum = 0;
    uint64_t seed;
    if (argc <= 1) {
	std::random_device rdev;
	seed = rdev();
	seed <<= 32;
	seed |= rdev();
    } else {
	seed = strtoul(argv[1], nullptr, 0);
    }
    rng_t rng(seed);
#if RNG_HAS_DISTANCE
    rng_t rng_copy = rng;
#endif
    Timer timer;
    timer.start("Test 1");
    for (uint32_t i = 0xffffffff; i > 0; --i) {
	uint32_t bval = bounded_rand(rng, i);
	assert(bval < i);
	sum += bval;
    }
    timer.done();

    std::cout << "Sum1 = " << sum << "\n";
    sum = 0;

    timer.start("Test 2");
    for (uint32_t j = 0; j < 0xffff; ++j) {
	for (uint32_t i = 0x0000ffff; i > 0; --i) {
	    uint32_t bval = bounded_rand(rng, i);
	    assert(bval < i);
	    sum += bval;
	}
    }
    timer.done();
    
    std::cout << "Sum2 = " << sum << "\n";
    sum = 0;
    
    timer.start("Test 3");
    for (uint32_t bit = 1; bit != 0; bit <<= 1) {
	for (uint32_t i = 0; i < 0x1000000; ++i) {
	    uint32_t bound = bit | (i & (bit - 1));
	    uint32_t bval = bounded_rand(rng, bound);
	    assert(bval < bound);
	    sum += bval;
	}
    }
    timer.done();
    std::cout << "Sum3 = " << sum << "\n";

    
#if RNG_HAS_DISTANCE
    std::cout << rng - rng_copy << " numbers used" << "\n";
#endif
}
	
