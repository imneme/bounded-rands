#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

/*
 * A C++ implementation of a simple timer helper class.
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

#include <chrono>
#include <iostream>

struct Timer
{
    std::chrono::time_point<std::chrono::system_clock> start_;
    const char* what_ = nullptr;

    Timer() = default;

    Timer(const char* what)
    {
	start(what);
    }

    void start(const char* what) {
	what_ = what;
	std::cout << what_ << " started...\n";
	start_ = std::chrono::system_clock::now();
    }

    void done() {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start_;
	std::cout << what_ << " completed (" << elapsed_seconds.count()
		  << " seconds)\n";
    }
};

#endif // TIMER_HPP_INCLUDED
