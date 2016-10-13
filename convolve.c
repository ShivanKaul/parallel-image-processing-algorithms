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

int get_index_for_new_image(int i, int j, int channel) {
  return width * 4 * (i - 1) + 4 * (j - 1) + channel;
}

int get_index(int curRow, int curCol, int rowPos, int colPos, int channel) {
  // printf("\n Channel is %d", channel);
  // printf("curRow is %d\n", curRow);
  // printf("curCol is %d\n", curCol);
  // printf("width is %d\n", width);
  // printf("rowPos is %d\n", rowPos);
  // printf("colPos is %d\n", colPos);
  // printf("Calculating index: %d", (4 * width * curRow + ((4 * width) * rowPos) ) + (4 * curCol + (4 * colPos)) + channel);
  // printf("Getting num: %d\n", image[(4 * width * curRow + ((4 * width) * rowPos) ) + (4 * curCol + (4 * colPos)) + channel]);
  return (4 * width * curRow + ((4 * width) * rowPos) ) + (4 * curCol + (4 * colPos)) + channel;
}

// unsigned char* get_neighbours(int curRow, int curCol, int channel) {

//   unsigned char *neighbours = malloc(9 * sizeof(unsigned char));
//   for (int i = 0; i < 3; i++) {
//     for (int j = 0; j < 3; j++)
//     {
//       neighbours[i]
//     }
//   }


//   unsigned char neighbours[] = {
//     image[get_index(curRow, curCol, -1, -1, channel)],
//     image[get_index(curRow, curCol, -1, 0, channel)],
//     image[get_index(curRow, curCol, -1, 1, channel)],

//     image[get_index(curRow, curCol, 0, -1, channel)],
//     image[get_index(curRow, curCol, 0, 0, channel)],
//     image[get_index(curRow, curCol, 0, 1, channel)],

//     image[get_index(curRow, curCol, 1, -1, channel)],
//     image[get_index(curRow, curCol, 1, 0, channel)],
//     image[get_index(curRow, curCol, 1, 1, channel)]
//   };
//   return &neighbours;
// }

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

  // check if neighbours logic works
  // for (int i = 0; i < 3; i++)
  // {
  //   for (int j = 0; j < 3; j++)
  //   {
  //     printf("R: %d ", image[4 * width * i + 4 * j + 0]);
  //     // printf("G: %d ", image[4 * width * i + 4 * j + 1]);
  //     // printf("B: %d ", image[4 * width * i + 4 * j + 2]);
  //     // printf("A: %d ", image[4 * width * i + 4 * j + 3]);
  //   }
  //   printf("\n");
  // }

  // printf("NOW PRINTING NEIGHBOURS!!!!\n");



  // test code
  // height = 4;
  // width = 4;
  // unsigned char image[4][4] = {
  //   -4,-4,-4,-4,  0,0,0,0,  4,4,4,4,  3,3,3,3,
  //   2,2,2,2,  2,2,2,2,  -1,-1,-1,-1,  -5,-5,-5,-5,
  //   4,4,4,4,  -1,-1,-1,-1,  0,0,0,0,  0,0,0,0,
  //   4,4,4,4,  4,4,4,4,  1,1,1,1,  2,2,2,2
  // };




  // process image
  // #pragma omp parallel for num_threads(NUM_THREADS)
  int counter = 0;
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
      // unsigned char neighboursA[] =
      // { image[get_index(i, j, -1, -1, A)],
      //   image[get_index(i, j, -1, 0, A)],
      //   image[get_index(i, j, -1, 1, A)],

      //   image[get_index(i, j, 0, -1, A)],
      //   image[get_index(i, j, 0, 0, A)],
      //   image[get_index(i, j, 0, 1, A)],

      //   image[get_index(i, j, 1, -1, A)],
      //   image[get_index(i, j, 1, 0, A)],
      //   image[get_index(i, j, 1, 1, A)]
      // };

      // for (int i = 0; i < 3; i++)
      // {
      //   for (int j = 0; j < 3; j++)
      //   {
      //     printf("R: %d ", neighboursR[i * 3 + j]);
      //   }
      //   printf("\n");
      // }

      // return;


      new_image[get_index_for_new_image(i, j, R)] = normalize(multiply_with_weights(neighboursR));
      // counter++;
      new_image[get_index_for_new_image(i, j, G)] = normalize(multiply_with_weights(neighboursG));
      // counter++;
      new_image[get_index_for_new_image(i, j, B)] = normalize(multiply_with_weights(neighboursB));
      // counter++;
      // new_image[get_index_for_new_image(i, j, A)] = normalize(multiply_with_weights(neighboursA));
      new_image[get_index_for_new_image(i, j, A)] = image[get_index(i, j, 0, 0, A)];
      // counter++;



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
// printf("counter is %d\n", counter);
  printf("height is %d\n", height);
  printf("width is %d\n", width);
  printf("height - 2 x width - 2 * 4 is %d\n", (height - 2) * (width - 2) * 4);

// printf("This is new image:\n");

// for(int i =0; i < height - 2; i++) {
//   for (int j = 0; j < width -2; j++) {
//     printf("R: %d ", new_image[i][j+0]);
//     printf("G: %d ", new_image[i][j+1]);
//     printf("B: %d ", new_image[i][j+2]);
//     printf("A: %d ", new_image[i][j+3]);
//   }
//   printf("\n");
// }


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
