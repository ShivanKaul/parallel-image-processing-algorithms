#include "convolve.h"

// Global variables because ugh C
int width;
unsigned char *image;

// Helper function for the matrix multiplication
float convolveOp(unsigned char neighbours[])
{
  float sum = 0.0;
  int i, j;
  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 3; j++)
    {
      sum += neighbours[i * 3 + j] * w[i][j];
    }
  }
  return sum;
}

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

// Helper function to get the neighbours
unsigned char *get_neighbours(int i, int j, int channel)
{
  unsigned char *neighbours = malloc(9 * sizeof(unsigned char));
  int counter = 0;
  int r, c;
  for (r = -1; r <= 1; r++)
  {
    for (c = -1; c <= 1; c++)
    {
      neighbours[counter] = image[get_index(i, j, r, c, channel)];
      counter++;
    }
  }
  return neighbours;
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
  unsigned error;
  unsigned char *new_image;
  unsigned height;

  // Read in image
  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if (error)
    printf("Error %u in lodepng: %s\n", error, lodepng_error_text(error));
  new_image = malloc((width - 4) * (height - 4) * 4 * sizeof(unsigned char)); // m - 2 x n - 2

// process image
int i, j;
#pragma omp parallel for num_threads(NUM_THREADS)
  for (i = 1; i < height - 1; i++)
  {
#pragma omp parallel for num_threads(NUM_THREADS)
    for (j = 1; j < width - 1; j++)
    {
      unsigned char *neighboursR = get_neighbours(i, j, R);
      unsigned char *neighboursG = get_neighbours(i, j, G);
      unsigned char *neighboursB = get_neighbours(i, j, B);

      new_image[get_index_for_new_image(i, j, R)] = clamp(convolveOp(neighboursR));
      new_image[get_index_for_new_image(i, j, G)] = clamp(convolveOp(neighboursG));
      new_image[get_index_for_new_image(i, j, B)] = clamp(convolveOp(neighboursB));
      new_image[get_index_for_new_image(i, j, A)] = 255; // idgaf
    }
  }

  // Save to disk
  lodepng_encode32_file(output_filename, new_image, width - 2, height - 2);

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
    total_time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
  }
  double avg_time_spent = total_time_spent / NUM_REPS;
  printf("Average time spent in convolve with %d threads (after running %d times) : %f s\n", NUM_THREADS, NUM_REPS, avg_time_spent);

  return 0;
}
