/* Disciplina: Programacao Concorrente */
/* Profa.: Silvana Rossetto */
/* Eduarda Leal de Carvalho */
/* Laboratório 2 - Atividade 3*/
/* Codigo: Produto interno de dois vetor de floats lidos de um arquivo binário */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

// Estrutura para passar dados para as threads
typedef struct {
    int start;
    int end;
    float *a;
    float *b;
    double *resultado_parcial;
} t_Args;

void* calcular_produto_interno(void *arg) {
    t_Args *args = (t_Args *)arg;
    double produto_interno = 0.0;
    for (int i = args->start; i < args->end; i++) {
        produto_interno += args->a[i] * args->b[i];
    }
    *args->resultado_parcial = produto_interno;
    return NULL;
}

int main(int argc, char *argv[]) {

    clock_t inicio, fim;
    double tempo_execucao;

    // Marcar o início do tempo
    inicio = clock();

    if (argc != 3) {
        printf("Uso: %s <numero_de_threads> <arquivo_binario>\n", argv[0]);
        return 1;
    }

    int nthreads = atoi(argv[1]);  // Número de threads
    const char *filename = argv[2];
    int N;
    float *a, *b;
    double produto_interno_sequencial, produto_interno_concorrente = 0.0;

    // Ler o arquivo binário
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    fread(&N, sizeof(int), 1, file);

    a = malloc(N * sizeof(float));
    b = malloc(N * sizeof(float));

    fread(a, sizeof(float), N, file);
    fread(b, sizeof(float), N, file);
    fread(&produto_interno_sequencial, sizeof(double), 1, file);
    fclose(file);

    // Criar threads e dados para threads
    pthread_t threads[nthreads];
    t_Args thread_data[nthreads];
    double resultados_parciais[nthreads];

    int elements_per_thread = N / nthreads;

    for (int i = 0; i < nthreads; i++) {
        thread_data[i].start = i * elements_per_thread;
        thread_data[i].end = (i == nthreads - 1) ? N : (i + 1) * elements_per_thread;
        thread_data[i].a = a;
        thread_data[i].b = b;
        thread_data[i].resultado_parcial = &resultados_parciais[i];
        pthread_create(&threads[i], NULL, calcular_produto_interno, &thread_data[i]);
    }

    // Aguardar todas as threads terminarem
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
        produto_interno_concorrente += resultados_parciais[i];
    }

    // Calcular a variação relativa
    double variacao_relativa = fabs((produto_interno_sequencial - produto_interno_concorrente) / produto_interno_sequencial);

    printf("Produto interno (sequencial): %lf\n", produto_interno_sequencial);
    printf("Produto interno (concorrente): %lf\n", produto_interno_concorrente);
    printf("Variação relativa: %lf\n", variacao_relativa);

    free(a);
    free(b);

      // Marcar o fim do tempo
    fim = clock();

    // Calcular o tempo de execução em segundos
    tempo_execucao = ((double) (fim - inicio)) / CLOCKS_PER_SEC;

    // Imprimir o tempo de execução
    printf("Tempo de execução: %f segundos\n", tempo_execucao);

    return 0;
}
