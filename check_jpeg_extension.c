#include "old_photo-lib.h"

/******************************************************************************
 * valid_extension_flag()
 *
 * Argumentos:
 *      filename - pointer da string que será verificada
 * Retorna:
 *      1 - se for um .jpeg
 *      0 - caso contrário
 *
 * Descrição:
 *      Verifica se o nome atual da foto possui a extensão .jpeg
 *
 *****************************************************************************/
int valid_extension_flag(char *filename){
    char *p_extension = strrchr(filename, '.'); //Ve onde está o ultimo '.'

    if (p_extension && strcmp(p_extension, EXTENSION_NAME) == 0) {
        return 1;  //extensão válida
    }
    return 0;  //extensão inválida
}


/******************************************************************************
 * count_jpegs()
 *
 * Argumentos:
 *      input_directory - diretoria onde as imagens estão
 *
 * Retorna:
 *      counter, o numero de jpegs que há na pasta
 *      -1, nao deu para contar
 *
 * Descrição:
 *      Esta função percorre os.jpegs de uma dada pasta e conta
 *      quantos estão la dentro
 *
 *****************************************************************************/

int count_jpegs(char *input_directory) {
    DIR *d;
    struct dirent *f;
    int counter = 0;

    d = opendir(input_directory);
    if (!d) {
        printf("nao deu para abrir a diretoria\n");
        return -1;
    }

    while ((f = readdir(d)) != NULL) {
        if (valid_extension_flag(f->d_name)) {
            counter++;
        }
    }

    closedir(d);
    return counter;
}




/******************************************************************************
 * store_names()
 *
 * Argumentos:
 *      files           - pointer para o array que possui nome de todos os ficheiros
 *      count           - número total .jpegs
 *      input_directory - diretoria onde estao as imagens
 *
 * Retorna:
 *      0 em caso de sucesso.
 *      -1 se nao der para ler a diretoria
 *
 * Descrição:
 *      é alocada com a diretoria
 *
 *****************************************************************************/
int store_names(char ***files, int count, char *input_directory) {
    DIR *d;
    struct dirent *f;
    int index = 0;

    //abre a diretoria
    d = opendir(input_directory);
    if (!d) {
        printf("n deu para abrir a diretoria\n");
        return -1;
    }

    //aloca memoria para acber lado tudo
    *files = malloc(count * sizeof(char *));
    if (!*files) {
        printf("nao deu para alocar a memoria\n");
        closedir(d);
        return -1;
    }


    while ((f = readdir(d)) != NULL) {
        if (valid_extension_flag(f->d_name)) {
            (*files)[index] = strdup(f->d_name);
            if (!(*files)[index]) {
                printf("nao foi possivel escrever o nome da imagem no files\n");
                closedir(d);
                return -1;
            }
            index++;
        }
    }

    closedir(d);
    return 0;
}
