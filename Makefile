all: old-photo-parallel-B

old-photo-parallel-B: main.c check_jpeg_extension.c sorts.c input_and_output.c threads.c image-lib.c
	gcc main.c check_jpeg_extension.c sorts.c input_and_output.c threads.c image-lib.c -g -o old-photo-parallel-B -lgd -lpthread

clean:
	rm old-photo-parallel-B

clean_all: clean
	rm -fr ./*-dir

run_all: all
	./old-photo-parallel-B

