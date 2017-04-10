#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/*
    This is the solution which aims
    for ultimate speed and speed only.
    It assumes that input is in valid format
    and the file is readable by program.

    Tested on linux
    Should work on mac
    Won't work on Windows

    gcc version:
    $ gcc --version
    gcc (GCC) 6.3.1 20170306

    Compile:
    $ gcc w6.c -o w6 -Ofast

    Run:
    $ ./w6 input.txt
    40401

    Benchmark:
    $ perf stat -r 100000 -d ./w6 input.txt > /dev/null

    Performance counter stats for './w6 ../input.txt' (100000 runs):

          0.234332      task-clock:u (msec)       #    0.628 CPUs utilized            ( +-  0.03% )
                 0      context-switches:u        #    0.000 K/sec
                 0      cpu-migrations:u          #    0.000 K/sec
                56      page-faults:u             #    0.239 M/sec                    ( +-  0.01% )
           385,286      cycles:u                  #    1.644 GHz                      ( +-  0.09% )  (0.00%)
           819,069      instructions:u            #    2.13  insn per cycle           ( +-  0.00% )
           201,042      branches:u                #  857.937 M/sec                    ( +-  0.00% )
             4,966      branch-misses:u           #    2.47% of all branches          ( +-  0.01% )
           104,740      L1-dcache-loads:u         #  446.974 M/sec                    ( +-  0.00% )
     <not counted>      L1-dcache-load-misses:u                                       ( +-  0.91% )  (62.62%)
     <not counted>      LLC-loads:u                                                   (0.00%)
     <not counted>      LLC-load-misses:u                                             (0.00%)

       0.000373166 seconds time elapsed                                          ( +-  0.04% )

    Author:
    Henri Koski
    heppu @ github
*/

int main (int argc, char *argv[]) {
    struct stat sb;
    off_t len;
    char *p;
    unsigned int fd;

    // Map file to memory using GNU mmap.
    // In this way on my setup the parser loop
    // runs about 2x faster than using fopen.
    fd = open(argv[1], O_RDONLY);
    fstat(fd, &sb);
    S_ISREG(sb.st_mode);
    p = (char*) mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    // Parse ascii bytes to unsigned integers
    // After quite a lot of bencmarking found this to be
    // the most efficient way af parsing unsigned integers.
    // I was kind of hoping that data set would have much bigger
    // so I could have used gpu to do parsing but the over head
    // for data set this small made it slower. =(
    unsigned int num = 0, index = 0, start = 0;
    unsigned int nums[sb.st_size / 2];

    for (len = 0; len < sb.st_size; len++) {
        if (p[len] == 10) {
            nums[index++] = num;
            num = 0;
            continue;
        }
        num = (num << 3) + (num << 1) + (p[len] - 48);
    }
    nums[index++] = num;
    munmap(p, sb.st_size);

    // Algorithm to find largest square from histogram.
    // While this implementation has higher complexity
    // then the stack based implementation it is actually
    // faster. I tried also implementing unsafe stack
    // without bound checks but still this one was faster.
    // After deeper investigation I found that at least on my
    // i7-7500U CPU amount of branch prediction misses
    // (6.87% vs 2.00%) and amount of instructions per cycle
    // (1.52 vs 2.30) were the main reasons why this almost 2x
    // complex solution is actually faster.
    unsigned int size = 0, width = 0, height = 1;
    unsigned int prevSmallest = nums[0], prevSmallestIndex = 0;

    for (unsigned int i = 0; i < index; i++) {
        width++;

        if (nums[i] < height) {
            width = 0;
            prevSmallest = 0xFFFFFFFF;
            continue;
        }

        if (nums[i] <= prevSmallest) {
            prevSmallest = nums[i];
            prevSmallestIndex = i;
        }

        if (width == height) {
            size = width;
            height++;
            if (prevSmallest < height + 1) {
                width = 0;
                i = prevSmallestIndex;
            }
        }
    }

    printf("%d\n", size * size);
    return 0;
}