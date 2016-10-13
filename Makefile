pool: pool.c pool.h
	gcc -g  -o pool pool.c lodepng.c  -fopenmp

poolmac: pool.c pool.h
	gcc-6 -g  -o pool pool.c lodepng.c  -fopenmp

rectify: rectify.c rectify.h
	gcc -g  -o rectify rectify.c lodepng.c  -fopenmp

rectifymac: rectify.c rectify.h
	gcc-6 -g  -o rectify rectify.c lodepng.c  -fopenmp

convolve: convolve.c convolve.h
	gcc -g  -o convolve convolve.c lodepng.c  -fopenmp

convolvemac: convolve.c convolve.h
	gcc-6 -g  -o convolve convolve.c lodepng.c  -fopenmp

test:
	gcc test_equality.c lodepng.c -lm

testmac:
	gcc test_equality.c lodepng.c

clean:
	rm -r rectify.dSYM
	rm rectify
	rm pool
	rm shivan*
