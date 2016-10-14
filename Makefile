UNAME := $(shell uname)
CC = gcc 
ifeq ($(UNAME), Darwin)
	CC = gcc-6
endif

pool: pool.c pool.h
	$(CC) -g  -o pool pool.c lodepng.c  -fopenmp

rectify: rectify.c rectify.h
	$(CC) -g  -o rectify rectify.c lodepng.c  -fopenmp

convolve: convolve.c convolve.h
	$(CC) -g -o convolve convolve.c lodepng.c  -fopenmp

test:
	$(CC) -std=c99 test_equality.c lodepng.c -lm

clean:
	-rm rectify -f
	-rm pool -f
	-rm convolve -f
	-rm shivan* -f
	-rm -r rectify.dSYM -f
