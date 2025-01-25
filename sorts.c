#include "old_photo-lib.h"



/******************************************************************************
 * sort_alfabeticaly()
 *
 * Argumentos:
 *      a - pointer para o elemento a comparar
 *      b - outro pointer para outro elemento a comparar
 * Retorna:
 *      <0 - se stra<strb
 *       0 - se ambas stra=strb
 *      >0 - se stra>strb
 *
 * Descrição:
 *      Compara duas strings alfabeticamente
 *
 *****************************************************************************/
int sort_alfabeticaly(const void *a, const void *b){
    char *stra = *(char **)a;
    char *strb = *(char **)b;
    return strcmp(stra, strb);
}



/******************************************************************************
 * sort_size()
 *
 * Argumentos:
 *      a - pointer para o elemento a comparar
 *      b - outro pointer para outro elemento a comparar
 * Retorna:
 *      <0 - se stat_a.st_size < stat_b.st_size
 *       0 - se stat_a.st_size = stat_b.st_size
 *      >0 - se stat_a.st_size > stat_b.st_size
 *
 * Descrição:
 *      Compara o tamanho de duas imagens
 *
 *****************************************************************************/
int sort_size(const void *a, const void *b){
    struct stat stat_a, stat_b;
    stat(*(char **)a, &stat_a);
    stat(*(char **)b, &stat_b);
    return (int)(stat_a.st_size - stat_b.st_size);
}



/******************************************************************************
 * sort()
 *
 * Argumentos:
 *      files  - array de strings que queremos ordenar
 *      order  - tipo de ordenaçao
 *      nn_files - número total de .jepgs no array files
 *
 * Retorna:
 *      nada
 *
 * Descrição:
 *      ordena files de acordo com o criterio de sort escolhido.
 *      se a ordem for -name, faz sort alfabeticamente
 *      se for -size, faz pelo tamanho dos ficheiros
 *
 *****************************************************************************/
void sort(char **files, char *order, int nn_files){

    if (strcmp(order, "-name") == 0) {
        qsort(files, nn_files, sizeof(char *), sort_alfabeticaly);
    } else if (strcmp(order, "-size") == 0) {
        qsort(files, nn_files, sizeof(char *), sort_size);
    }
}
