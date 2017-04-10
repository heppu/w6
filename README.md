# w6
My solution for finding biggest square in histogram for [this](https://github.com/wunderdogsw/wunderpahkina-vol6) code challenge.

## Compile
$ gcc w6.c -o w6 -Ofast

## Run
$ ./w6 input.txt
40401

## Benchmark
$ perf stat -r 100000 -d ./w6 ../input.txt > /dev/null
