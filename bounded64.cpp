/*
 * A C++ implementation methods and benchmarks for random numbers in a range
 * (64-bit version)
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

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    std::uniform_int_distribution<uint64_t> dist(0, range-1);

    return dist(rng);
}

#elif USE_BIASED_FP_MULT_LDEXP

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    long double zeroone = ldexpl(rng(), -64);
    return range * zeroone;
}

#elif USE_BIASED_FP_MULT_SCALE

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    long double zeroone = 0x1.0p-64l * rng();
    return range * zeroone;
}

#elif USE_BIASED_MOD

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    return rng() % range;
}

#elif USE_DEBIASED_DIV

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t divisor = ((-range) / range) + 1;
    if (divisor == 0)
	return 0;
    for (;;) {
        uint64_t val = rng() / divisor;
        if (val < range)
            return val;
    }
}

#elif USE_DEBIASED_MODx2

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t t = (-range) % range;
    for (;;) {
        uint64_t r = rng();
        if (r >= t)
            return r % range;
    }
}

#elif USE_DEBIASED_MODx2_MOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t t = -range;
    if (t >= range) {
	t -= range;
	if (t >= range) 
	    t %= range;
    }
    for (;;) {
        uint64_t r = rng();
        if (r >= t)
            return r % range;
    }
}

#elif USE_DEBIASED_MODx2_TOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t r = rng();
    if (r < range) {
	uint64_t t = (-range) % range;
	while (r < t)
	    r = rng();
    }
    return r % range;
}

#elif USE_DEBIASED_MODx2_TOPT_BOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t r = rng();
    if (range >= 1ul<<63) {
	while(r >= range)
	    r = rng();
	return r;
    }
    if (r < range) {
	uint64_t t = (-range) % range;
	while (r < t)
	    r = rng();
    }
    return r % range;
}

#elif USE_DEBIASED_MODx2_TOPT_MOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t r = rng();
    if (r < range) {
	uint64_t t = -range;
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

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t r = rng();
    if (r < range) {
	uint64_t t = -range;
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

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x, r;
    do {
	x = rng();
	r = x % range;
    } while (x - r > uint64_t(-range));
    return r;
}

#elif USE_DEBIASED_MODx1_BOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x, r;
    if (range >= 1ul<<63) {
	do {
	    r = rng();
	} while (r >= range);
	return r;
    }
    do {
	x = rng();
	r = x % range;
    } while (x - r > uint64_t(-range));
    return r;
}

#elif USE_DEBIASED_MODx1_MOPT
      
static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x, r;
    do {
	x = rng();
	r = x;
	if (r >= range) {
	    r -= range;
	    if (r >= range)
		r %= range;
	}
    } while (x - r > uint64_t(-range));
    return r;
}

#elif USE_BIASED_INT_MULT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x = rng();
    __uint128_t m = __uint128_t(x) * __uint128_t(range);
    return m >> 64;
}



#elif USE_DEBIASED_INT_MULT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t t = (-range) % range;
    uint64_t l;
    __uint128_t m;
    do {
	uint64_t x = rng();
	m = __uint128_t(x) * __uint128_t(range);
	l = uint64_t(m);
    } while (l < t);
    return m >> 64;
}


#elif USE_DEBIASED_INT_MULT_TOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x = rng();
    __uint128_t m = __uint128_t(x) * __uint128_t(range);
    uint64_t l = uint64_t(m);
    if (l < range) {
	uint64_t t = (-range) % range;
	while (l < t) {
	    x = rng();
	    m = __uint128_t(x) * __uint128_t(range);
	    l = uint64_t(m);
	}
    }
    return m >> 64;
}

#elif USE_DEBIASED_INT_MULT_TOPT_BOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x = rng();
    if (range >= 1ul<<63) {
	while(x >= range)
	    x = rng();
	return x;
    }
    __uint128_t m = __uint128_t(x) * __uint128_t(range);
    uint64_t l = uint64_t(m);
    if (l < range) {
	uint64_t t = (-range) % range;
	while (l < t) {
	    x = rng();
	    m = __uint128_t(x) * __uint128_t(range);
	    l = uint64_t(m);
	}
    }
    return m >> 64;
}

#elif USE_DEBIASED_INT_MULT_TOPT_MOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x = rng();
    __uint128_t m = __uint128_t(x) * __uint128_t(range);
    uint64_t l = uint64_t(m);
    if (l < range) {
	uint64_t t = -range;
	if (t >= range) {
	    t -= range;
	    if (t >= range) 
		t %= range;
	}
	while (l < t) {
	    x = rng();
	    m = __uint128_t(x) * __uint128_t(range);
	    l = uint64_t(m);
	}
    }
    return m >> 64;
}

#elif USE_DEBIASED_INT_MULT_TOPT_MOPT_BOPT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t x = rng();
    if (range >= 1ul<<63) {
	while(x >= range)
	    x = rng();
	return x;
    }
    __uint128_t m = __uint128_t(x) * __uint128_t(range);
    uint64_t l = uint64_t(m);
    if (l < range) {
	uint64_t t = -range;
	t -= range;
	if (t >= range) 
	    t %= range;
	while (l < t) {
	    x = rng();
	    m = __uint128_t(x) * __uint128_t(range);
	    l = uint64_t(m);
	}
    }
    return m >> 64;
}

#elif USE_BITMASK

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    uint64_t mask = ~uint64_t(0);
    --range;
    mask >>= __builtin_clzl(range|1);
    uint64_t x;
    do {
	x = rng() & mask;
    } while (x > range);
    return x;
}

#elif USE_BITMASK_ALT

static uint64_t bounded_rand(rng_t& rng, uint64_t range) {
    --range;
    unsigned int zeros = __builtin_clzl(range|1);
    uint64_t mask = (~uint64_t(0)) >> zeros;
    for (;;) {
	uint64_t r = rng();
	uint64_t v = r & mask;
	if (v <= range)
	    return v;
	unsigned int shift = 32;
	while (zeros >= shift) {
	    r >>= shift;
	    v = r & mask;
	    if (v <= range)
		return v;
	    shift = 64 - (64 - shift)/2;
	}
    }
}

#endif

int main(int argc, char* argv[])
{
    pcg_extras::pcg128_t sum = 0;
    using pcg_extras::operator<<;

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

    // Large shuffle
    timer.start("Test 1");
    for (uint32_t i = 0xffffffff; i > 0; --i) {
	uint64_t bound = (uint64_t(i)<<32) | i;
	uint64_t bval = bounded_rand(rng, bound );
	assert(bval < bound);
	sum += bval;
    }
    timer.done();
    std::cout << "Sum1 = " << sum << "\n";

    // Small shuffle
    sum = 0;
    timer.start("Test 2");
    for (uint64_t i = 0xffffffff; i > 0; --i) {
	uint64_t bval = bounded_rand(rng, i);
	assert(bval < i);
	sum += bval;
    }
    timer.done();
    std::cout << "Sum2 = " << sum << "\n";

    // All-ranges shuffle
    sum = 0;
    timer.start("Test 3");
    for (uint64_t bit = 1; bit != 0; bit <<= 1) {
	for (uint32_t i = 0; i < 0x800000; ++i) {
	    uint64_t bound = bit | (i & (bit - 1));
	    uint64_t bval = bounded_rand(rng, bound);
	    assert(bval < bound);
	    sum += bval;
	}
    }
    timer.done();
    std::cout << "Sum3 = " << sum << "\n";

    // Small constant
    sum = 0;
    timer.start("Test 4");
    for (uint32_t i = 0; i < 0x80000000; ++i) {
	uint64_t bval = bounded_rand(rng, 52);
	assert(bval < 52);
	sum += bval;
    }
    timer.done();
    std::cout << "Sum4 = " << sum << "\n";

    // Large constant
    sum = 0;
    timer.start("Test 5");
    for (uint32_t i = 0; i < 0x80000000; ++i) {
	uint64_t bval = bounded_rand(rng, uint64_t(-52));
	assert(bval < uint64_t(-52));
	sum += bval;
    }
    timer.done();
    std::cout << "Sum4 = " << sum << "\n";

#if RNG_HAS_DISTANCE
    std::cout << rng - rng_copy << " numbers used" << "\n";
#endif
}
