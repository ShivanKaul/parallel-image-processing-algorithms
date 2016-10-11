rectify: rectify.c rectify.h
	gcc-6 -g  -o rectify rectify.c lodepng.c

rectify_parallel: rectify.c rectify.h
	gcc-6 -g -o rectify rectify.c -fopenmp lodepng.c

compiletest:
	gcc test_equality.c lodepng.c

clean:
	rm -r rectify.dSYM
	rm rectify
	rm shivan*