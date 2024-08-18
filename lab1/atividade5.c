/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Laboratório 1 - Atividade 5*/
/* Codigo: Incrementando os valores de um vetor usando threads em C */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//Struct para armazenar os argumentos da thread
typedef struct {
    int *vector;
    int start;
    int end;
    int idThread;
} t_Args;

//Função executada pelas thread
void* add1(void* arg) {
    t_Args* args = (t_Args*)arg;
    printf("\n--Thread %d processando posições de %d a %d\n", args->idThread, args->start, args->end - 1);
    for (int i = args->start; i < args->end; i++) {
        printf("Antes: vector[%d] = %d, ", i, args->vector[i]);
        args->vector[i] += 1;
        printf("Depois: vector[%d] = %d\n", i, args->vector[i]);
    }

    pthread_exit(NULL);
}

//Função para inicializar o vetor com valores aleatórios
void initialize_vector(int* vector, int n) {
    printf("--Inicializando o vetor com valores aleatórios:\n");
    for (int i = 0; i < n; i++) {
        vector[i] = rand() % 100;
        printf("vector[%d] = %d\n", i, vector[i]);
    }
    printf("\n");
}

//Função para verificar se o resultado está correto
void verify(int* vector, int* original_vector, int n) {
    int correct = 1;
    printf("\n--Comparando o vetor original com o vetor incrementado:\n");
    for (int i = 0; i < n; i++) {
        printf("Posição %d: original = %d, incrementado = %d\n", i, original_vector[i], vector[i]);
        if (vector[i] != original_vector[i] + 1) {
            correct = 0;
        }
    }
    if (correct) {
        printf("\nResultado correto!\n");
    } else {
        printf("\nErro no resultado.\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <tamanho do vetor N> <numero de threads M>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    int *vector = (int*)malloc(N * sizeof(int));
    int *original_vector = (int*)malloc(N * sizeof(int));

    if (vector == NULL || original_vector == NULL) {
         printf("--Falha ao alocar memória");
        return 1;
    }

    initialize_vector(vector, N);

    // Copiar vector inicial para verificação posterior
    for (int i = 0; i < N; i++) {
        original_vector[i] = vector[i];
    }

    pthread_t threads[M];
    t_Args t_data[M];
    int elements_per_thread = N / M;
    int remainder = N % M;

    printf("--Criando as threads...\n");
    for (int i = 0; i < M; i++) {
        t_data[i].vector = vector;
        t_data[i].start = i * elements_per_thread;
        t_data[i].end = (i + 1) * elements_per_thread;
        t_data[i].idThread = i;

        if (i == M - 1) {
            // A última thread trata o resto dos elementos
            t_data[i].end += remainder;
        }

        if (pthread_create(&threads[i], NULL, add1, (void*)&t_data[i])) {
            printf("--ERRO: pthread_create()\n");
            return 2;
        }
    }

    // Esperando todas as threads terminarem
    for (int i = 0; i < M; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("--ERRO: pthread_join() da thread %d\n", i);
        }
    }

    verify(vector, original_vector, N);

}
