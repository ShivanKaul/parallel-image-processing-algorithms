#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


void process(char *input_filename, char *output_filename, int NUM_THREADS);
