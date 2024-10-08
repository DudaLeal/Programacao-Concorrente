// main_writer_priority.c
#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include <pthread.h>
#include "timer.h"

#define QTDE_OPS 10000000 // Quantidade de operações sobre a lista
#define QTDE_INI 100       // Quantidade de inserções iniciais na lista
#define MAX_VALUE 100      // Valor máximo a ser inserido

// Lista compartilhada iniciada
struct list_node_s* head_p = NULL;
// Quantidade de threads no programa
int nthreads;

// Variáveis para controle de leitura/escrita com prioridade para escrita
int read_count = 0;
int write_request = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_read = PTHREAD_COND_INITIALIZER;

// Função que simula as operações de leitura e escrita com prioridade para escrita
void* tarefa(void* arg) {
    long int id = (long int) arg;
    int op;
    int in, out, read;
    in = out = read = 0;

    for (long int i = id; i < QTDE_OPS; i += nthreads) {
        op = rand() % 100;

        if (op < 98) { // Operação de leitura
            pthread_mutex_lock(&mutex);
            while (write_request > 0) { // Espera enquanto há solicitações de escrita
                // Uncomment for debugging:
                // printf("Thread %ld aguardando para ler\n", id);
                pthread_cond_wait(&can_read, &mutex);
            }
            read_count++; // Incrementa contador de leituras ativas
            if (read_count == 1)
                pthread_mutex_lock(&write_mutex); // Bloqueia escrita
            pthread_mutex_unlock(&mutex);

            // Realiza leitura
            Member(i % MAX_VALUE, head_p);
            read++;

            pthread_mutex_lock(&mutex);
            read_count--; // Leitura finalizada
            if (read_count == 0)
                pthread_mutex_unlock(&write_mutex); // Libera escrita
            pthread_mutex_unlock(&mutex);

        } else { // Operação de inserção ou remoção (escrita)
            pthread_mutex_lock(&mutex);
            write_request++; // Marca solicitação de escrita
            pthread_mutex_unlock(&mutex);

            pthread_mutex_lock(&write_mutex); // Bloqueia escrita

            // Realiza escrita
            if (op < 99) {
                // Inserção
                Insert(i % MAX_VALUE, &head_p);
                in++;
            } else {
                // Remoção
                Delete(i % MAX_VALUE, &head_p);
                out++;
            }

            pthread_mutex_lock(&mutex);
            write_request--; // Escrita finalizada
            if (write_request == 0)
                pthread_cond_broadcast(&can_read); // Acorda leitores esperando
            pthread_mutex_unlock(&mutex);

            pthread_mutex_unlock(&write_mutex); // Libera escrita
        }
    }

    printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
    pthread_exit(NULL);
}

/* Função principal */
int main(int argc, char* argv[]) {
    pthread_t* tid;
    double ini, fim, delta;

    if (argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    // Insere os primeiros elementos na lista
    for (int i = 0; i < QTDE_INI; i++)
        Insert(i % MAX_VALUE, &head_p);

    // Aloca espaço de memória para o vetor de identificadores de threads no sistema
    tid = malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        printf("--ERRO: malloc()\n");
        return 2;
    }

    // Tomada de tempo inicial
    GET_TIME(ini);

    // Cria as threads
    for (long int i = 0; i < nthreads; i++) {
        if (pthread_create(tid + i, NULL, tarefa, (void*)i)) {
            printf("--ERRO: pthread_create()\n");
            return 3;
        }
    }

    // Aguarda as threads terminarem
    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(*(tid + i), NULL)) {
            printf("--ERRO: pthread_join()\n");
            return 4;
        }
    }

    // Tomada de tempo final
    GET_TIME(fim);
    delta = fim - ini;
    printf("Tempo: %lf\n", delta);

    // Libera o espaço de memória do vetor de threads
    free(tid);
    // Libera o espaço de memória da lista
    Free_list(&head_p);

    return 0;
}
