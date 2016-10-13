#include "convolve.h"

int rectOp(unsigned char i)
{
  return (i < 127) ? 127 : i;
}

float multiply_with_weights(unsigned char neighbours[])
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

int normalize(float num) {
  int normalized = (int)num;
  normalized = normalized > 255 ? 255 : normalized;
  return (normalized < 0) ? 0 : normalized;
}

void process(char *input_filename, char *output_filename, int NUM_THREADS)
{
  unsigned error;
  // unsigned char *image, *new_image;
  unsigned char  *new_image;
  unsigned width, height;

  // Read in image
  // error = lodepng_decode32_file(&image, &width, &height, input_filename);
  // if (error)
  //   printf("Error %u in lodepng: %s\n", error, lodepng_error_text(error));
  new_image = malloc((width - 2) * (height - 2) * 4 * sizeof(unsigned char)); // m - 2 x n - 2

  // check if neighbours logic works
  // for (int i = 0; i < 3; i++)
  // {
  //   for (int j = 0; j < 3; j++)
  //   {
  //     printf("R: %d ", image[4 * width * i + 4 * j + 0]);
  //     printf("B: %d ", image[4 * width * i + 4 * j + 1]);
  //     printf("G: %d ", image[4 * width * i + 4 * j + 2]);
  //     printf("A: %d ", image[4 * width * i + 4 * j + 3]);
  //   }
  //   printf("\n");
  // }

  // printf("NOW PRINTING NEIGHBOURS!!!!\n");



  // test code
  height = 4;
  width = 4;
  unsigned char image[4][4] = { 
    -4,-4,-4,-4,  0,0,0,0,  4,4,4,4,  3,3,3,3,
    2,2,2,2,  2,2,2,2,  -1,-1,-1,-1,  -5,-5,-5,-5,
    4,4,4,4,  -1,-1,-1,-1,  0,0,0,0,  0,0,0,0,
    4,4,4,4,  4,4,4,4,  1,1,1,1,  2,2,2,2
  };




  // process image
  // #pragma omp parallel for num_threads(NUM_THREADS)
  int counter = 0;
  for (int i = 1; i <= height - 2; i++)
  {
    // #pragma omp parallel for num_threads(NUM_THREADS)
    for (int j = 1; j <= (width - 2); j++)
    {
      int adder = 0;
      unsigned char neighboursR[] = {image[(4 * width * i - (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j + 4) + adder],

                           image[(4 * width * i) + (4 * j - 4) + adder],
                           image[(4 * width * i) + (4 * j) + adder],
                           image[(4 * width * i) + (4 * j + 4) + adder],

                           image[(4 * width * i + (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j + 4) + adder]
                          };
      adder++;
      unsigned char neighboursG[] = {image[(4 * width * i - (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j + 4) + adder],

                           image[(4 * width * i) + (4 * j - 4) + adder],
                           image[(4 * width * i) + (4 * j) + adder],
                           image[(4 * width * i) + (4 * j + 4) + adder],

                           image[(4 * width * i + (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j + 4) + adder]
                          };
      adder++;
      unsigned char neighboursB[] = {image[(4 * width * i - (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j + 4) + adder],

                           image[(4 * width * i) + (4 * j - 4) + adder],
                           image[(4 * width * i) + (4 * j) + adder],
                           image[(4 * width * i) + (4 * j + 4) + adder],

                           image[(4 * width * i + (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j + 4) + adder]
                          };
      adder++;
      unsigned char neighboursA[] = {image[(4 * width * i - (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i - (4 * width)) + (4 * j + 4) + adder],

                           image[(4 * width * i) + (4 * j - 4) + adder],
                           image[(4 * width * i) + (4 * j) + adder],
                           image[(4 * width * i) + (4 * j + 4) + adder],

                           image[(4 * width * i + (4 * width)) + (4 * j - 4) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j) + adder],
                           image[(4 * width * i + (4 * width)) + (4 * j + 4) + adder]
                          };

      // new_image[4 * width * (i - 1) + 4 * (j - 4) + 0] = normalize(multiply_with_weights(neighboursR));

      new_image[counter] = normalize(multiply_with_weights(neighboursR));
      counter++;
      new_image[counter] = normalize(multiply_with_weights(neighboursG));
      counter++;
      new_image[counter] = normalize(multiply_with_weights(neighboursB));
      counter++;
      new_image[counter] = normalize(multiply_with_weights(neighboursA));
      counter++;
      // new_image[4 * width * i + 4 * j + 0] = convolveOp(i, j, );                       // R
      // new_image[4 * width * i + 4 * j + 1] = rectOp(image[4 * width * i + 4 * j + 1]); // G
      // new_image[4 * width * i + 4 * j + 2] = rectOp(image[4 * width * i + 4 * j + 2]); // B
      // new_image[4 * width * i + 4 * j + 3] = image[4 * width * i + 4 * j + 3];         // A
      // int blah = 0;
      // for (; blah < 9; blah++)
      // {
      //   printf("%d  ", neighbours[blah]);
      // }
      // return;
    }
  }
  printf("counter is %d\n", counter);
  printf("height is %d\n", height);
  printf("width is %d\n", width);
  printf("height - 2 x width - 2 * 4 is %d\n", (height - 2) * (width - 2) * 4);

  printf("This is new image:\n");

  for(int i =0; i < height - 2; i++) {
    for (int j = 0; j < width -2; j++) {
      printf("R: %d ", new_image[i][j+0]);
      printf("G: %d ", new_image[i][j+1]);
      printf("B: %d ", new_image[i][j+2]);
      printf("A: %d ", new_image[i][j+3]);
    }
    printf("\n");
  }


  // #pragma omp parallel num_threads(NUM_THREADS)
  //   {
  //     int tid = omp_get_thread_num();
  //     int chunk_size = (height * width * 4) / omp_get_num_threads();
  //     int start_idx = tid * chunk_size;
  //     int end_idx = (tid == omp_get_num_threads() - 1) ? (height * width * 4) : start_idx + chunk_size;
  //     int idx;
  //     for (idx = start_idx; idx < end_idx; idx++)
  //     {
  //       new_image[idx] = rectOp(image[idx]);
  //     }
  //   }

  lodepng_encode32_file(output_filename, new_image, width, height);

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
  printf("Average time spent in convolve with %d threads (after running %d times) : %f s", NUM_THREADS, NUM_REPS, avg_time_spent);

  return 0;
}
