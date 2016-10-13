#include "convolve.h"

int width;
unsigned char *image;


int rectOp(unsigned char i)
{
  return (i < 127) ? 127 : i;
}

float multiply_with_weights(unsigned char neighbours[])
{
  float sum = 0.0;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      sum += neighbours[i * 3 + j] * w[i][j];
    }
  }
  return sum;
}

int get_index_for_new_image(int i, int j, int channel) {
  return width * 4 * (i - 1) + 4 * (j - 1) + channel;
}

int get_index(int curRow, int curCol, int rowPos, int colPos, int channel) {
  return (4 * width * curRow + ((4 * width) * rowPos) ) + (4 * curCol + (4 * colPos)) + channel;
}

int normalize(float num) {
  int normalized = (int)num;
  normalized = normalized > 255 ? 255 : normalized;
  return (normalized < 0) ? 0 : normalized;
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
  new_image = malloc((width - 2) * (height - 2) * 4 * sizeof(unsigned char)); // m - 2 x n - 2

  // process image
  // #pragma omp parallel for num_threads(NUM_THREADS)
  for (int i = 1; i < height - 1; i++)
  {
    // #pragma omp parallel for num_threads(NUM_THREADS)
    for (int j = 1; j < width - 1; j++)
    {
      unsigned char neighboursR[] = {
        image[get_index(i, j, -1, -1, R)],
        image[get_index(i, j, -1, 0, R)],
        image[get_index(i, j, -1, 1, R)],

        image[get_index(i, j, 0, -1, R)],
        image[get_index(i, j, 0, 0, R)],
        image[get_index(i, j, 0, 1, R)],

        image[get_index(i, j, 1, -1, R)],
        image[get_index(i, j, 1, 0, R)],
        image[get_index(i, j, 1, 1, R)]
      };

      unsigned char neighboursG[] = {
        image[get_index(i, j, -1, -1, G)],
        image[get_index(i, j, -1, 0, G)],
        image[get_index(i, j, -1, 1, G)],

        image[get_index(i, j, 0, -1, G)],
        image[get_index(i, j, 0, 0, G)],
        image[get_index(i, j, 0, 1, G)],

        image[get_index(i, j, 1, -1, G)],
        image[get_index(i, j, 1, 0, G)],
        image[get_index(i, j, 1, 1, G)]
      };
      unsigned char neighboursB[] = {
        image[get_index(i, j, -1, -1, B)],
        image[get_index(i, j, -1, 0, B)],
        image[get_index(i, j, -1, 1, B)],

        image[get_index(i, j, 0, -1, B)],
        image[get_index(i, j, 0, 0, B)],
        image[get_index(i, j, 0, 1, B)],

        image[get_index(i, j, 1, -1, B)],
        image[get_index(i, j, 1, 0, B)],
        image[get_index(i, j, 1, 1, B)]
      };


      new_image[get_index_for_new_image(i, j, R)] = normalize(multiply_with_weights(neighboursR));
      new_image[get_index_for_new_image(i, j, G)] = normalize(multiply_with_weights(neighboursG));
      new_image[get_index_for_new_image(i, j, B)] = normalize(multiply_with_weights(neighboursB));
      new_image[get_index_for_new_image(i, j, A)] = 255;
    }
  }

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
