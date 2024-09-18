#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long int soma = 0; //variável compartilhada entre as threads
pthread_mutex_t mutex; //variável de lock para exclusão mútua
pthread_cond_t cond; //variável de condição para sincronização
int multiplo10_count = 0; // contador de múltiplos de 10 impressos

//função executada pelas threads
void *ExecutaTarefa(void *arg) {
    long int id = (long int) arg;
    printf("Thread : %ld está executando...\n", id);

    for (int i = 0; i < 100000; i++) {
        //--entrada na SC
        pthread_mutex_lock(&mutex);
        
        //--SC (seção crítica)
        soma++; //incrementa a variável compartilhada 

        // Verifica se soma é múltiplo de 10 e sinaliza a thread extra
        if (soma % 10 == 0 && multiplo10_count < 20) {
            pthread_cond_signal(&cond); // Sinaliza que um múltiplo de 10 foi alcançado
            pthread_cond_wait(&cond, &mutex); // Aguarda a thread extra liberar
        }

        //--saída da SC
        pthread_mutex_unlock(&mutex);
    }
    printf("Thread : %ld terminou!\n", id);
    pthread_exit(NULL);
}

//função executada pela thread de log
void *extra(void *args) {
    printf("Extra: está executando...\n");

    while (multiplo10_count < 20) {
        pthread_mutex_lock(&mutex);

        // Aguarda até que 'soma' seja múltiplo de 10
        while (soma % 10 != 0 || multiplo10_count >= 20) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Imprime o valor de soma e incrementa o contador de múltiplos de 10
        printf("soma = %ld\n", soma);
        multiplo10_count++;

        // Libera as threads para continuar
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);
    }

    printf("Extra: terminou!\n");
    pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
    pthread_t *tid; //identificadores das threads no sistema
    int nthreads; //qtde de threads (passada linha de comando)

    //--le e avalia os parâmetros de entrada
    if (argc < 2) {
        printf("Digite: %s <número de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    //--aloca as estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * (nthreads + 1));
    if (tid == NULL) {
        puts("ERRO--malloc");
        return 2;
    }

    //--inicializa o mutex e a variável de condição
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    //--cria as threads
    for (long int t = 0; t < nthreads; t++) {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *) t)) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    //--cria thread de log
    if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
        printf("--ERRO: pthread_create()\n");
        exit(-1);
    }

    //--espera todas as threads terminarem
    for (int t = 0; t < nthreads + 1; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join()\n");
            exit(-1);
        }
    }

    //--finaliza o mutex e a variável de condição
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    printf("Valor de 'soma' = %ld\n", soma);

    return 0;
}
