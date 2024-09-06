/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Eduarda Leal de Carvalho */
/* Laboratório 3 - Atividade 1*/
/* Codigo: Multiplicação de matrizes, de forma concorrente, lidas de um arquivo binário e armazenar o
resultado em um outro arquivo binário */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "timer.h"

// Estrutura para passar dados para threads
typedef struct {
    float *A, *B, *C;
    int N, M, P, thread_id, nthreads;
} t_Args;

// Função para carregar matriz de um arquivo binário
float* load_matrix(const char* filename, int* rows, int* cols) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }

    fread(rows, sizeof(int), 1, file);
    fread(cols, sizeof(int), 1, file);

    float* matrix = (float*)malloc((*rows) * (*cols) * sizeof(float));
    if (!matrix) {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }

    fread(matrix, sizeof(float), (*rows) * (*cols), file);

    fclose(file);
    return matrix;
}

// Função para salvar matriz em um arquivo binário
void save_matrix(const char* filename, float* matrix, int rows, int cols) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }

    fwrite(&rows, sizeof(int), 1, file);
    fwrite(&cols, sizeof(int), 1, file);
    fwrite(matrix, sizeof(float), rows * cols, file);

    fclose(file);
}

// Função de multiplicação para cada thread
void* multiply_thread(void* arg) {
    t_Args* args = (t_Args*)arg;
    int start = (args->thread_id * args->N) / args->nthreads;
    int end = ((args->thread_id + 1) * args->N) / args->nthreads;

    for (int i = start; i < end; ++i) {
        for (int j = 0; j < args->P; ++j) {
            args->C[i * args->P + j] = 0;
            for (int k = 0; k < args->M; ++k) {
                args->C[i * args->P + j] += args->A[i * args->M + k] * args->B[k * args->P + j];
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    double inicio, fim, delta;

    GET_TIME(inicio);

    if (argc != 5) {
        printf("Uso: %s <matrizA> <matrizB> <resultado> <threads>\n", argv[0]);
        return 1;
    }

    int nthreads = atoi(argv[4]);
    int N1, M1, N2, M2;
    float *A = load_matrix(argv[1], &N1, &M1);
    float *B = load_matrix(argv[2], &N2, &M2);


    if (M1 != N2) {
        fprintf(stderr, "Erro: Dimensões incompatíveis para multiplicação\n");
        return 2;
    }

    float* C = (float*)malloc(N1 * M2 * sizeof(float));
    if (!C) {
        fprintf(stderr, "Erro de alocao da memoria da matriz\n");
        return 3;
    }
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo inicializacao:%lf\n", delta);

    //multiplicacao da matriz pelo vetor
    GET_TIME(inicio);
    pthread_t threads[nthreads];
    t_Args thread_data[nthreads];

    for (int i = 0; i < nthreads; ++i) {
        thread_data[i] = (t_Args){A, B, C, N1, M1, M2, i, nthreads};
        pthread_create(&threads[i], NULL, multiply_thread, &thread_data[i]);
    }

    for (int i = 0; i < nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    save_matrix(argv[3], C, N1, M2);

    GET_TIME(fim)
    delta = fim - inicio;
    printf("Tempo multiplicacao (dimensao %d x %d) (nthreads %d): %lf\n", N1, M2, nthreads, delta);

    GET_TIME(inicio);
    free(A);
    free(B);
    free(C);
    GET_TIME(fim)
    delta = fim - inicio;
    printf("Tempo finalizacao:%lf\n", delta);

    return 0;
}
