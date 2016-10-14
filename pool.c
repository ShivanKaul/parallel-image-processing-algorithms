#include "pool.h"

#define MAX(a,b) ((a) > (b) ? a : b)

clock_t beginLoad;
clock_t endLoad;
clock_t beginStore;
clock_t endStore;

int poolOp(unsigned char *i, int p, unsigned width)
{
  return MAX(i[p], MAX(i[p + 4], MAX(i[p + (width * 4)], i[p + (width * 4) + 4])));
}

void process(char *input_filename, char *output_filename, int NUM_THREADS)
{
  beginLoad = clock();
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;

  // Read in image
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if (error)
    printf("Error %u in lodepng: %s\n", error, lodepng_error_text(error));
  new_image = malloc(width * height * sizeof(unsigned char));
  endLoad = clock();

#pragma omp parallel num_threads(NUM_THREADS)
  {
    int tid = omp_get_thread_num();
    int chunk_size = (height/2 * width/2 * 4) / omp_get_num_threads();
    int start_idx = tid * chunk_size;
    int end_idx = (tid == omp_get_num_threads() - 1) ? (height * width) : start_idx + chunk_size;
    int idx = start_idx;
    int pos = (2 * (idx / (width * 2)) * width * 4) + (2 * (idx%(width * 2))); //(2 * (idx % (width * 2)));
    //printf ("\n start indx = %i and end_idx = %i \n With pos %i with offset idx:pos %i:%i at tid: %i out of %i \n", start_idx, end_idx, pos, pos%4, idx%4, tid, omp_get_num_threads());
    //printf ("Supposed to be at %i but at %i with an offset of %i bits; %i pixels\n", idx*4, pos, pos - idx*4, (pos - idx*4)/4);
    //printf ("Originx: %i ; Originy: %i ....... NewX: %i (%i) ; NewY: %i\n", pos%(width*4), pos/(width*4), idx%(width*2), 2*(idx%(width*2)), idx/(width*2));
    //pos -= ((pos% 4 ) - (idx % 4));
    pos -= idx%4;
    if (idx%4 == 0 && tid != 0) {
        pos -= 4;
    }
    //pos -= ((tid/4) * 4);
    //printf ("Actually being at Originx: %i ; Originy: %i \n", pos%(width*4), pos/(width*4));
    //pos -= (pos%(width*4) - 2*(idx%(width*2)));
   // pos += 4 - ((pos% 4 ) - (idx % 4));
    for (idx = start_idx; idx < end_idx; idx++)
    {
      if (idx > 0 && idx % 4 == 0) {
        pos += 4;
      }
      if (idx % (width * 2) == 0 && idx != 0 || (pos / (width * 4)) % 2 ==1) {
        pos += width * 4;
      }
      int ang = poolOp(image, pos, width);
      new_image[idx] = ang;
      pos++;
    }
    //printf("\n %i \n", idx);
    //printf ("\ntid %i finished at pos: %i ; idx: %i \n Originx: %i ; Originy: %i ....... NewX: %i (%i) ; NewY: %i\n", tid, pos, idx, pos%(width*4), pos/(width*4), idx%(width*2), 2*(idx%(width*2)), idx/(width*2));
  }

  beginStore = clock();
  lodepng_encode32_file(output_filename, new_image, width/2, height/2);
  endStore = clock();

  free(image);
  free(new_image);
}

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    printf("Incorrect arguments! Input format: ./pool <name of input png> <name of output png> < # threads> \n");
    return;
  }
  int NUM_REPS = 1;
  if (argc == 5)
  {
    NUM_REPS = atoi(argv[4]);
  }
  char *input_filename = argv[1];
  char *output_filename = argv[2];
  int NUM_THREADS = atoi(argv[3]);

  clock_t begin, end;
  double total_time_spent = 0.0;
  int i;
  for (i = 0; i < NUM_REPS; i++)
  {
    begin = clock();
    process(input_filename, output_filename, NUM_THREADS);
    end = clock();
    total_time_spent += (double)(end - begin - (endLoad - beginLoad) - (endStore - beginStore)) / CLOCKS_PER_SEC;
  }
  double avg_time_spent = total_time_spent / NUM_REPS;
  printf("Average time spent in pool with %d threads (after running %d times) : %f s\n", NUM_THREADS, NUM_REPS, avg_time_spent);

  return 0;
}
