/*
Programa auxiliar para gerar um vetor de floats
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para gerar um número aleatório de ponto flutuante
float random_float() {
    return (float)rand() / RAND_MAX * 10.0;  // Gera um número entre 0 e 10
}

int main() {
    clock_t inicio, fim;
    double tempo_execucao;

    // Marcar o início do tempo
    inicio = clock();

    int N = 1000000;  // Dimensão dos vetores
    float *a = malloc(N * sizeof(float));
    float *b = malloc(N * sizeof(float));
    double produto_interno = 0.0;



    srand(time(NULL));

    // Gerar vetores aleatórios e calcular o produto interno
    for (int i = 0; i < N; i++) {
        a[i] = random_float();
        b[i] = random_float();
        produto_interno += a[i] * b[i];

        // printf("\nPara i = %d\n", i);
        // printf("VETOR A %f\n", a[i]);
        // printf("VETOR B %f\n", b[i]);
        // printf("PRODUTO INTERNO %lf\n", produto_interno);
    }

    // Salvar os vetores e o resultado em um arquivo binário
    FILE *file = fopen("dados.bin", "wb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    fwrite(&N, sizeof(int), 1, file);
    fwrite(a, sizeof(float), N, file);
    fwrite(b, sizeof(float), N, file);
    fwrite(&produto_interno, sizeof(double), 1, file);

    fclose(file);

    free(a);
    free(b);

    // Marcar o fim do tempo
    fim = clock();

    // Calcular o tempo de execução em segundos
    tempo_execucao = ((double) (fim - inicio)) / CLOCKS_PER_SEC;

    // Imprimir o tempo de execução
    printf("Tempo de execução: %f segundos\n", tempo_execucao);

    printf("Dados gerados e salvos em 'dados.bin'.\n");

    return 0;
}
