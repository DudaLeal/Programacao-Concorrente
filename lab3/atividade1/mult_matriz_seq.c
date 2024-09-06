/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Eduarda Leal de Carvalho */
/* Laboratório 3 - Atividade 1*/
/* Codigo: Multiplicação de matrizes, de forma sequencial, lidas de um arquivo binário e armazenar o
resultado em um outro arquivo binário */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"


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

// Função de multiplicação sequencial de matrizes
void multiply_matrices(float* A, float* B, float* C, int N, int M, int P) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < P; ++j) {
            C[i * P + j] = 0;
            for (int k = 0; k < M; ++k) {
                C[i * P + j] += A[i * M + k] * B[k * P + j];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    double inicio, fim, delta;

    GET_TIME(inicio);

    if (argc != 4) {
        printf("Uso: %s <matrizA> <matrizB> <resultado>\n", argv[0]);
        return 1;
    }

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

    multiply_matrices(A, B, C, N1, M1, M2);

    save_matrix(argv[3], C, N1, M2);
    GET_TIME(fim)
    delta = fim - inicio;
    printf("Tempo multiplicacao (dimensao %d x %d): %lf\n", N1, M2, delta);

    GET_TIME(inicio);
    free(A);
    free(B);
    free(C);
    GET_TIME(fim)
    delta = fim - inicio;
    printf("Tempo finalizacao:%lf\n", delta);

    return 0;
}
