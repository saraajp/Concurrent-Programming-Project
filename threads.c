#include "old_photo-lib.h"

int global_counter = 0;
pthread_mutex_t mutex;
struct timespec total_jpeg_time = {0}; //guarda o tempo total de processamento de todas as imagens já processadas

/******************************************************************************
 * s_key()
 *
 * Argumentos:
 *      arg - pointer para a estrutura thread_struct
 * Retorna:
 *      NULL - quando não existirem mais imagens a serem processadas
 *
 * Descrição:
 *      Calcula e imprime no terminal as estatísticas caso o utilizador clique
 *      na tecla S
 *
 *****************************************************************************/
void* s_key(void* arg) {

    //loop à espera caso seja precionada a tecla S
    while (global_counter<nn_files) {
        char c = getchar();
        if (c == 'S' || c == 's') {
            pthread_mutex_lock(&mutex);
            int nn_processed = global_counter;
            int nn_remaining = nn_files - nn_processed;
            pthread_mutex_unlock(&mutex);

            //print das estatísticas no terminal
            printf("\nNº de imagens processadas: %d\n", nn_processed);
            printf("Nº de imagens que faltam %d\n", nn_remaining);
            printf("Tempo médio das imagens já processadas: %fs\n", (float)total_jpeg_time.tv_sec/nn_processed);
        }

         //para garantir que o programa nao dependa da tecla S para terminar
        pthread_testcancel();
    }
    return NULL;
}



/******************************************************************************
 * thread_function()
 *
 * Argumentos:
 *      arg - pointer para a estrutura thread_struct
 * Retorna:
 *      NULL - quando a thread acaba o seu trabalho
 *
 * Descrição:
 *     A distribuição de imagens agora depende da disponibilidade das threads
 *      Se uma dada imagem ja tenha sido processada, é ignorada
 *
 *****************************************************************************/
void* thread_function(void* arg) {
    struct timespec start_thd_timer, end_thd_timer;//para calcular o tempo que uma thread demora a processar todas as imagens a sí atribuídas
    thread_struct *thread_properties = (thread_struct *) arg;
    clock_gettime(CLOCK_MONOTONIC, &start_thd_timer);

    gdImagePtr in_img, out_smoothed_img, out_contrast_img, out_textured_img, out_sepia_img;
    char in_file_name[100], out_file_name[100];//para gurdar os nomes das diretorias de leitura e escrita das imagens

    while (1) {
        int index;

        //fazer lock and unlock do mutex para atribuir trabalho a uma thread que nao se encontre no momento com imagens a processar
        pthread_mutex_lock(&mutex);

        //caso nao hajam mais imagens a processar, sai
        if (global_counter >= thread_properties->end_index) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        index = global_counter++;
        pthread_mutex_unlock(&mutex);


        printf("Thread %d a processar a imagem: %s\n", thread_properties->thread_id, thread_properties->files[index]);

        //variáveis para calcular o começo e fim do processamento de uma imagem
        struct timespec start_img_timer, end_img_timer;
        clock_gettime(CLOCK_MONOTONIC, &start_img_timer); //o processamento começa aqui

        //criaçao da diretoria de leitura
        snprintf(in_file_name, sizeof(in_file_name), "%s/%s", thread_properties->input_directory, thread_properties->files[index]);

        //criaçao da diretoria de output
        snprintf(out_file_name, sizeof(out_file_name), "%s%s", thread_properties->output_directory, thread_properties->files[index]);

        //ver se uma das imagens ja está na diretoria
        if (access(out_file_name, F_OK) != -1) {
            printf("Thread %d ignorar (já foi processado): %s\n", thread_properties->thread_id, out_file_name);
            continue;
        }


        /* load the input file */
        in_img = read_jpeg_file(in_file_name);
        //caso nao seja possivel ler a imagem
        if (in_img == NULL) {
            fprintf(stderr, "Thread %d não conseguiu ler a imagem %s\n", thread_properties->thread_id, thread_properties->files[index]);
            continue;
        }

        //aplicar as transformaçoes
        out_contrast_img = contrast_image(in_img);
        out_smoothed_img = smooth_image(out_contrast_img);
        out_textured_img = texture_image(out_smoothed_img, thread_properties->in_texture_img);
        out_sepia_img = sepia_image(out_textured_img);

        /* save processed image */
        if (write_jpeg_file(out_sepia_img, out_file_name) == 0) {
             fprintf(stderr, "Impossible to write %s image\n", out_file_name);
        }

        //fazer free da  memoria
        gdImageDestroy(out_smoothed_img);
        gdImageDestroy(out_sepia_img);
        gdImageDestroy(out_contrast_img);
        gdImageDestroy(in_img);

        //aqui acaba o processamento da imagem
        clock_gettime(CLOCK_MONOTONIC, &end_img_timer);

        //calculo do temppo que de morou para a imagem ser processada
        struct timespec img_time = diff_timespec(&end_img_timer, &start_img_timer);

        //atualização do tempo total do processamento de todas as imagens já processadas
        total_jpeg_time.tv_sec += img_time.tv_sec;
    }

    //parar a contage, do tempo paralelo
    clock_gettime(CLOCK_MONOTONIC, &end_thd_timer);
    thread_properties->timer = diff_timespec(&end_thd_timer, &start_thd_timer);

    return NULL;
}



/******************************************************************************
 * manage_threads()
 *
 * Argumentos:
 *      files         - matriz que contem o nome de todas as imagens .jpeg
 *      nn_files      - nº de ficheiros .jpeg da matriz files
 *      n_threads     - número de threads que existem
 *      input_directory - diretoria de onde estão imagens
 *      output_directory - diretoria onde queremos guardar as imagens processadas
 *      in_texture_img - textura que vai ser aplicada às imagens
 *      thread_properties - struct que contem as particularidades para cada thread
 *
 * Retorna:
 *      nada
 *
 * Descrição:
 *      Cria e faz join das threads trabalhadoras e extra
 *
 *****************************************************************************/


void manage_threads(char **files, int nn_files, int n_threads, char *input_directory, char *output_directory, gdImagePtr in_texture_img, thread_struct *thread_properties) {

    pthread_t threads[n_threads];
    pthread_t extra_thread;

    //criar o mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Falha a criar a mutex");
        exit(-1);
    }

    //criar a thread extra
    if (pthread_create(&extra_thread, NULL, s_key, NULL) != 0) {
       printf("Falha a criar a thread extra");
        exit(-1);
    }

    //criaçao e inicializaçao das threads trabalhadoras
    for (int i = 0; i < n_threads; i++) {
        thread_properties[i].files = files;
        thread_properties[i].input_directory = input_directory;
        thread_properties[i].output_directory = output_directory;
        thread_properties[i].in_texture_img = in_texture_img;
        thread_properties[i].start_index = 0;
        thread_properties[i].end_index = nn_files;
        thread_properties[i].thread_id = i;

        if (pthread_create(&threads[i], NULL, thread_function, (void*)&thread_properties[i]) != 0) {
            printf("Falha ao criar thread\n");
            exit(-1);
        }
    }

    //fazer join das threads trabalhadoras
    for (int i = 0; i < n_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Falha ao fazer join da thread\n");
            exit(-1);
        }
    }

    //para garantir que o programa nao dependa da tecla S para terminar
   pthread_cancel(extra_thread);

    //fazer join da thread extra
    if (pthread_join(extra_thread, NULL) != 0) {
        printf("Falha ao fazer join da thread extra\n");
        exit(-1);
    }

    //destroir o mutex
    pthread_mutex_destroy(&mutex);
}
