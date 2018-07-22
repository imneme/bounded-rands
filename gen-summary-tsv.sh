#!/bin/sh

./summarize.pl --method out/bounded32.*.out bounded32-methods.tsv
./summarize.pl --method out/bounded64.*.out bounded64-methods.tsv

./summarize.pl --method out/bounded32.mt19937*.*.out bounded32-methods-mt.tsv
./summarize.pl --method out/bounded64.mt19937*.*.out bounded64-methods-mt.tsv

./summarize.pl --prng out/bounded32.*.out bounded32-prngs.tsv
./summarize.pl --prng out/bounded64.*.out bounded64-prngs.tsv
