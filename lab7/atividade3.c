#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Declaração dos semáforos, buffers e variáveis globais
sem_t sem1, sem2, sem3;
char *buffer1;
char buffer2[1000];
int buffer_count = 0;
int file_end = 0;
int buffer_size;

// Função da thread 1: Lê linhas do arquivo
void *thread1_func(void *arg) {
    FILE *input = (FILE*) arg;
    while (fgets(buffer1, buffer_size + 1, input) != NULL) {
        sem_post(&sem2);
        sem_wait(&sem1);
    }
    fclose(input);
    file_end = 1;  // Marca fim do arquivo
    sem_post(&sem2);
    sem_post(&sem3);
    pthread_exit(NULL);
}

// Função da thread 2: Processa o buffer1 e coloca o resultado no buffer2
void *thread2_func(void *arg) {
    int index = 0, n = 0;
    int next_newline = 2 * n + 1;
    while (1) {
        sem_wait(&sem2);
        if (file_end) break;
        for (int i = 0; i < buffer_size; i++) {
            char current_char = buffer1[i];
            if (index == next_newline) {
                buffer2[buffer_count++] = '\n';
                index = 0;
                if (n < 10) {
                    n++;
                    next_newline = 2 * n + 1;
                } else {
                    next_newline = 10;
                }
            }
            buffer2[buffer_count++] = current_char;
            index++;
        }
        sem_post(&sem1);
    }
    pthread_exit(NULL);
}

// Função da thread 3: Imprime o buffer2
void *thread3_func(void *arg) {
    sem_wait(&sem3);
    buffer2[buffer_count] = '\0';
    printf("%s\n", buffer2);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t thread1, thread2, thread3;
    FILE *input;
    if (argc < 3) {
        printf("Uso: %s <tamanho do buffer> <arquivo de entrada>\n", argv[0]);
        return 1;
    }
    buffer_size = atoi(argv[1]);
    buffer1 = (char*) malloc((buffer_size + 1) * sizeof(char));  // Aloca buffer1
    input = fopen(argv[2], "r");

    // Inicializa semáforos
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);

    // Cria threads
    pthread_create(&thread1, NULL, thread1_func, (void*) input);
    pthread_create(&thread2, NULL, thread2_func, NULL);
    pthread_create(&thread3, NULL, thread3_func, NULL);

    // Espera threads terminarem
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    // Destroi semáforos
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);

    return 0;
}
