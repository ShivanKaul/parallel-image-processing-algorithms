/* Example of using lodepng to load, process, save image */
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>

int rectOp(unsigned char i)
{
    // printf("rectOp has : %d\n", i);
    return (i < 127) ? 127 : i;
}

void process(char* input_filename, char* output_filename, int NUM_THREADS)
{
  unsigned error;
  unsigned char *image, *new_image;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, input_filename);
  if(error) printf("Error %u in lodepng: %s\n", error, lodepng_error_text(error));
  new_image = malloc(width * height * 4 * sizeof(unsigned char));

  // process image
  unsigned char value;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {	

    	// value = image[4*width*i + 4*j];

	    new_image[4*width*i + 4*j + 0] = rectOp(image[4*width*i + 4*j + 0]); // R
      // printf("new image 0 has : %d\n", new_image[4*width*i + 4*j + 0]);
	    new_image[4*width*i + 4*j + 1] = rectOp(image[4*width*i + 4*j + 1]); // G
      // printf("new image 1 has : %d\n", new_image[4*width*i + 4*j + 1]);
	    new_image[4*width*i + 4*j + 2] = rectOp(image[4*width*i + 4*j + 2]); // B
      // printf("new image 2 has : %d\n", new_image[4*width*i + 4*j + 2]);
	    new_image[4*width*i + 4*j + 3] = image[4*width*i + 4*j + 3]; // A
    }
  }

  lodepng_encode32_file(output_filename, new_image, width, height);

  free(image);
  free(new_image);
}

int main(int argc, char *argv[])
{
  if (argc < 4) {
    printf("Incorrect arguments! Input format: ./rectify <name of input png> <name of output png> < # threads> \n");
  }
  char* input_filename = argv[1];
  char* output_filename = argv[2];
  int NUM_THREADS = atoi(argv[3]);

  process(input_filename, output_filename, NUM_THREADS);

  return 0;
}
