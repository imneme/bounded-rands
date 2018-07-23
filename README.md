# Methods and Benchmarks for Random Numbers in a Range

Implements most known methods for random numbers in a range, and provides
some simple benchmarks.

See http://www.pcg-random.org/posts/bounded-rands.html

## Building

Run

    sh download-gists.sh
    sh gen-makefile.sh
    make -j 6

## Running all tests

    sh gen-tests.sh
    make -f Makefile.test -j 3
    sh gen-summary-tsv.sh

