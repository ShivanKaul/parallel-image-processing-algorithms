pool: pool.c pool.h
	gcc -g  -o pool pool.c lodepng.c  -fopenmp

rectify: rectify.c rectify.h
	gcc -g  -o rectify rectify.c lodepng.c  -fopenmp

compiletest:
	gcc test_equality.c lodepng.c

clean:
	rm -r rectify.dSYM
	rm rectify
	rm shivan*
