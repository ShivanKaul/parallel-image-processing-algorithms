#include "convolve.h"

// Global variables because ugh C
int width;
unsigned char *image;
clock_t beginLoad;
clock_t endLoad;
clock_t beginStore;
clock_t endStore;

// Helper function for getting index for the new image
int get_index_for_new_image(int i, int j, int channel)
{
  return (width - 2) * 4 * (i - 1) + 4 * (j - 1) + channel;
}

// Helper function for getting index for old image
// rowPos and colPos can be -1, 0, 1
int get_index(int curRow, int curCol, int rowPos, int colPos, int channel)
{
  return (4 * width * curRow + ((4 * width) * rowPos)) + (4 * curCol + (4 * colPos)) + channel;
}

// Helper function for the matrix multiplication
float convolveOp(int i, int j, int channel)
{
  float sum = 0.0;
  int r, c;
  for (r = -1; r <= 1; r++)
  {
    for (c = -1; c <= 1; c++)
    {
      sum += image[get_index(i, j, r, c, channel)] * w[r + 1][c + 1];
    }
  }
  return sum;
}

// Clamp the output
int clamp(float num)
{
  int clamped = (int)num;
  clamped = clamped > 255 ? 255 : clamped;
  return (clamped < 0) ? 0 : clamped;
}

void process(char *input_filename, char *output_filename, int NUM_THREADS)
{
  beginLoad = clock();
  unsigned error;
  unsigned char *new_image;
  unsigned height;

  // Read in image
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if (error)
    printf("Error %u in lodepng: %s\n", error, lodepng_error_text(error));
  new_image = malloc((width - 2) * (height - 2) * 4 * sizeof(unsigned char)); // m - 2 x n - 2
  endLoad = clock();

  // process image
  #pragma omp parallel num_threads(NUM_THREADS)
  {
    int tid = omp_get_thread_num();
    int chunk_size = ((height - 2) * (width - 2)) / omp_get_num_threads();
    int start_idx = tid * chunk_size + (width) + 1;
    int end_idx = (tid == omp_get_num_threads() - 1) ? (width * (height - 1)) - 1 : start_idx + chunk_size;
    int idx;
    for (idx = start_idx; idx < end_idx; idx++)
    {
      if (idx % width == 0) {
        idx++;
      }
      else if (idx % width == width - 1) {
        idx += 2;
      }
      if (idx >= end_idx) {
        break;
      }
      new_image[get_index_for_new_image(idx / (width), (idx % (width)), R)] = clamp(convolveOp(idx / width, (idx % (width)), R));
      new_image[get_index_for_new_image(idx / (width), (idx % (width)), G)] = clamp(convolveOp(idx / width, (idx % (width)), G));
      new_image[get_index_for_new_image(idx / (width), (idx % (width)), B)] = clamp(convolveOp(idx / width, (idx % (width)), B));
      new_image[get_index_for_new_image(idx / (width), (idx % (width)), A)] = 255; // idgaf
    }
  }

  beginStore = clock();
  // Save to disk
  lodepng_encode32_file(output_filename, new_image, width - 2, height - 2);
  endStore = clock();

  free(image);
  free(new_image);
}

int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    printf("Incorrect arguments! Input format: ./convolve <name of input png> <name of output png> < # threads> \n");
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
  printf("Average time spent in convolve with %d threads (after running %d times) : %f s\n", NUM_THREADS, NUM_REPS, avg_time_spent);

  return 0;
}
