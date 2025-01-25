#include "old_photo-lib.h"

/******************************************************************************
 * main()
 *
 * Arguments: (none)
 * Returns: 0 in case of sucess, positive number in case of failure
 * Side-Effects: creates thumbnail, resized copy and watermarked copies
 *               of images
 *
 * Description: implementation of the complex serial version
 *              This application only works for a fixed pre-defined set of files
 *
 *****************************************************************************/

//NOTE: todos os comentários e printfs em inglês nos vários ficheiros .c foram
//fornecidos pelo corpo docente

int nn_files = 0;
int main(int argc, char* argv[]) {
    struct timespec start_time_total, end_time_total;
    struct timespec start_time_seq, end_time_seq;
    struct timespec start_time_par, end_time_par;

    clock_gettime(CLOCK_MONOTONIC, &start_time_total);
    clock_gettime(CLOCK_MONOTONIC, &start_time_seq);

    /* array containing the names of files to be processed */
    char **files = NULL;

    /* file name of the image created and to be saved on disk */
    char output_txt[100];
    char output_directory[100];
    char *input_directory = NULL;
    int n_threads = 0;
    char *order = NULL;

    //leitura dos parametros de entrada, se forem inválidos o programa acaba
    if (parameters(argc, argv, &input_directory, &n_threads, &order) == 1) {
        printf("fechar programa\n");
        clock_gettime(CLOCK_MONOTONIC, &end_time_seq);
        clock_gettime(CLOCK_MONOTONIC, &end_time_total);
        return 0;
    }

    //1ª leitura da diretoria
    //obetivo: contar o nº de ficheiros do tipo .jpeg
    nn_files = count_jpegs(input_directory);
    if (nn_files <= 0) {
        printf("Nao foi possivel contar os ficheiros da diretoria %s\n", input_directory);
        exit(-1);
    }

    printf("\nNº de ficheiros: %d\n", nn_files);


    //2ª leitura da diretoria
    //objetivo: guardar os nomes dos .jpegs em char**files
    if (store_names(&files, nn_files, input_directory) != 0) {
        printf("Erro a guardar os nomes dos .jpegs\n");
        exit(-1);
    }

    //faz sort consoante o que foi pedido nos parametros
    sort(files, order, nn_files);

    //print da ordem após sort
    printf("Sorted jpegs:\n");
    for (int i = 0; i < nn_files; i++) {
        printf("%s\n", files[i]);
    }


    //criaçao da diretoria onde serao guardadas as fotos
    strcpy(output_directory, argv[1]);
    strcat(output_directory, OLD_IMAGE_DIR);
    printf("%s", output_directory);


    if (create_directory(output_directory) == 0) {
        printf("Impossible to create %s directory\n", output_directory);
        exit(-1);
    }


    gdImagePtr in_texture_img = read_png_file("./paper-texture.png");
    if (!in_texture_img) {
        printf("A paper texture nao está na pasta\n");
        exit(-1);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time_seq);
    clock_gettime(CLOCK_MONOTONIC, &start_time_par);

    thread_struct *thread_properties = NULL;
    thread_properties = malloc(n_threads * sizeof(thread_struct));
    if (!thread_properties) {
        printf("Nao foi possivel alocar\n");
        exit(-1);
    }

    //cria, faz join e executa o proposito das threads
    manage_threads(files, nn_files, n_threads, input_directory, output_directory, in_texture_img, thread_properties);

    clock_gettime(CLOCK_MONOTONIC, &end_time_par);
    clock_gettime(CLOCK_MONOTONIC, &end_time_total);

    struct timespec par_time = diff_timespec(&end_time_par, &start_time_par);
    struct timespec seq_time = diff_timespec(&end_time_seq, &start_time_seq);
    struct timespec total_time = diff_timespec(&end_time_total, &start_time_total);

    printf("\tseq \t %10jd.%09ld\n", seq_time.tv_sec, seq_time.tv_nsec);
    printf("\tpar \t %10jd.%09ld\n", par_time.tv_sec, par_time.tv_nsec);
    printf("total \t %10jd.%09ld\n", total_time.tv_sec, total_time.tv_nsec);

    //escreve o tempo total, sequencial e de cada thread num ficheiro de texto
    output_file(output_directory, n_threads, total_time, seq_time, par_time, thread_properties, argv[3]);

    //liberta memoria alocada
    for (int i = 0; i < nn_files; i++) {
        free(files[i]);
    }
    free(files);
    free(thread_properties);

    exit(0);
}
