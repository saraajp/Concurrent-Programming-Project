#include "old_photo-lib.h"
#include <time.h>


/******************************************************************************
 * parameters()
 *
 * Argumentos:
 *      argc      - número de argumentos
 *      argv      - array de strings com os argumentos
 *      directory - aponta para o nome da diretoria
 *      n_threads - aponta para a variavel que guarda o nº de threads a criar
 *      order     - aponta para a variavel que gaurda o tipo de ordenação
 * Retorna:
 *      0 - se os argumentos forem todos válidos
 *      1 - se pelo menos um argumento nao for valido, ou faltarem ou estiverem a mais
 *
 * Descrição:
 *      Processa os argumentos de entrada do código. Verifica se são o número certo
 *      e se são válidos
 *
 *****************************************************************************/
int parameters(int argc, char* argv[], char** directory, int* n_threads, char** order){

    //se os argumentos nao forem exatamente 4
    if (argc != 4){
        printf("Args inválidos\n");
        return 1;
    }

    *directory = argv[1];
    *n_threads = atoi(argv[2]);
    *order = argv[3];


    printf("(1)diretoria: %s\n", *directory);

    //caso o nº de threads for invalido
    if (*n_threads <= 0) {
        printf("numero de threads invalido\n");
        return 1;
    }

    //caso o tipo de ordenaçao nao for valido
    if (strcmp(*order, "-name") != 0 && strcmp(*order, "-size") != 0) {
        printf("n é valida a ordenaçao\n");
        return 1;
    }


    return 0;
}



/******************************************************************************
 * output_file()
 *
 * Argumentos:
 *      output_directory - nome do ficheiro de output
 *      n_threads         - nº de threads
 *      seq_time          - tempo de execução sequencial
 *      total_time        - tempo de execução total
 *      thread_properties - struct com informaçao sobre as threads
 *      order             - -name ou -size
 * Retorna:
 *      nada
 *
 * Descrição:
 *      Escreve os tempos de execução num ficheiro .txt
 *
 *****************************************************************************/
void output_file(char* output_directory, int n_threads, struct timespec total_time, struct timespec seq_time, struct timespec par_time, thread_struct* thread_properties, char* order) {
    FILE* file;
    char output_txt[100];

    //criar o nome do ficheiro txt
    sprintf(output_txt, "%s/timing_B_%d%s.txt", output_directory, n_threads, order);


    file = fopen(output_txt, "w");
    if (file == NULL) {
        printf("Nao foi possivel abrir o ficheiro de output\n");
        return;
    }


    //escrever o tempo de execução individual de cada thread
    for (int i = 0; i < n_threads; i++) {
        fprintf(file, "Thread nº %d :\t%10jd.%09ld\n", i, thread_properties[i].timer.tv_sec, thread_properties[i].timer.tv_nsec);
    }

    //escrever o tempo do código sequencial total
    fprintf(file, "seq\t\t%10jd.%09ld\n", seq_time.tv_sec, seq_time.tv_nsec);

    //escrever o tempo do código paralelo total
    fprintf(file, "par\t\t%10jd.%09ld\n", par_time.tv_sec, par_time.tv_nsec);

    //escrever o tempo total
    fprintf(file, "tempo total\t\t%10jd.%09ld\n", total_time.tv_sec, total_time.tv_nsec);


    fclose(file);
}
