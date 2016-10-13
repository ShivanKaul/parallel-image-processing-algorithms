#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "wm.h"
#define R 0
#define G 1
#define B 2
#define A 3

void process(char *input_filename, char *output_filename, int NUM_THREADS);
