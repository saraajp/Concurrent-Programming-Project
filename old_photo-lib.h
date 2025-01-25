#include <gd.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include "image-lib.h"
#include <unistd.h>


typedef struct {
    struct timespec timer;
    int start_index;
    int end_index;
    char **files;
    char *input_directory;
    char *output_directory;
    gdImagePtr in_texture_img;
    int thread_id;
} thread_struct;


#define OLD_IMAGE_DIR "/old_photo_PAR_B/"
#define EXTENSION_NAME ".jpeg"
extern int nn_files;



int valid_extension_flag(char *filename);
int parameters(int argc, char* argv[], char** directory, int* n_threads, char** order);
int count_jpegs(char *input_directory);
int store_names(char ***files, int count, char *input_directory);
int sort_alfabeticaly(const void *a, const void *b);
int sort_size(const void *a, const void *b);
void sort(char **files, char *order, int nn_files);
void* thread_function(void* arg);
void manage_threads(char **files, int nn_files, int n_threads, char *input_directory, char *output_directory, gdImagePtr in_texture_img, thread_struct *thread_properties);
void output_file(char* output_directory, int n_threads, struct timespec total_time, struct timespec seq_time, struct timespec par_time, thread_struct* thread_properties, char* order);
