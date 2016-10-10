rectify: rectify.c
	gcc-6 -g  -o rectify rectify.c lodepng.c

rectify_parallel: rectify.c
	gcc-6 -g -o rectify rectify.c -fopenmp lodepng.c

clean:
	rm -r rectify.dSYM
	rm rectify
	rm shivan*