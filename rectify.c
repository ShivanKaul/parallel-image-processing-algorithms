#include "rectify.h"

clock_t beginLoad;
clock_t endLoad;
clock_t beginStore;
clock_t endStore;

int rectOp(unsigned char i)
{
  return (i < 127) ? 127 : i;
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
  new_image = malloc(width * height * 4 * sizeof(unsigned char));
  endLoad = clock();
// process image
// #pragma omp parallel for num_threads(NUM_THREADS)
//   for (int i = 0; i < height; i++)
//   {
//     for (int j = 0; j < width; j++)
//     {
//       new_image[4 * width * i + 4 * j + 0] = rectOp(image[4 * width * i + 4 * j + 0]); // R
//       new_image[4 * width * i + 4 * j + 1] = rectOp(image[4 * width * i + 4 * j + 1]); // G
//       new_image[4 * width * i + 4 * j + 2] = rectOp(image[4 * width * i + 4 * j + 2]); // B
//       new_image[4 * width * i + 4 * j + 3] = image[4 * width * i + 4 * j + 3];         // A
//     }
//   }

#pragma omp parallel num_threads(NUM_THREADS)
  {
    int tid = omp_get_thread_num();
    int chunk_size = (height * width * 4) / omp_get_num_threads();
    int start_idx = tid * chunk_size;
    int end_idx = (tid == omp_get_num_threads() - 1) ? (height * width * 4) : start_idx + chunk_size;
    int idx;
    for (idx = start_idx; idx < end_idx; idx++)
    {
      new_image[idx] = rectOp(image[idx]);
    }
  }

  beginStore = clock();
  lodepng_encode32_file(output_filename, new_image, width, height);
  endStore = clock();

  free(image);
  free(new_image);
}

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    printf("Incorrect arguments! Input format: ./rectify <name of input png> <name of output png> < # threads> \n");
    return 1;
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
  printf("Average time spent in rectify with %d threads (after running %d times) : %f s\n", NUM_THREADS, NUM_REPS, avg_time_spent);

  return 0;
}
