# Laboratório 6 - Padrão Leitores e Escritores com Prioridade para Escrita

Este repositório contém a implementação do **padrão leitores e escritores com prioridade para escrita**, desenvolvido como parte do Laboratório 6 da disciplina de **Programação Concorrente** (ICP-361). O objetivo desta implementação é garantir que as operações de escrita tenham prioridade sobre as operações de leitura, utilizando sincronização com `pthread_rwlock_t` e condicionais de pthread.

## Objetivos
1. Implementar o padrão de leitores-escritores com prioridade para escrita, assegurando que as operações de escrita não sejam bloqueadas por leituras.
2. Avaliar o comportamento do programa ao manipular uma lista encadeada compartilhada por várias threads.
3. Medir o desempenho da aplicação conforme o número de threads aumenta.

## Estrutura do Projeto

- **`list_int.h`**: Cabeçalho que define as funções para manipular uma lista encadeada de inteiros.
- **`list_int.c`**: Implementação das funções de manipulação da lista encadeada.
- **`main_writer_priority.c`**: Programa principal que implementa a lógica de leitores-escritores com prioridade para escrita.
- **`timer.h`**: Macro para medir o tempo de execução das operações com precisão de microssegundos.

## Compilação e Execução

### Compilação

Para compilar o programa, utilize o seguinte comando no terminal:

```bash
gcc -o main_writer_priority main_writer_priority.c list_int.c -lpthread
```

### Execução

Para executar o programa, forneça o número de threads como argumento:

```bash
./main_writer_priority <numero_de_threads>
```

Por exemplo, para rodar com 4 threads:

```bash
./main_writer_priority 4
```

## Lógica de Implementação

### Prioridade para Escrita

O padrão de leitores e escritores com prioridade para escrita foi implementado utilizando as seguintes variáveis de controle:

- **`read_count`**: Contador de leitores ativos.
- **`write_request`**: Contador de solicitações de escrita.
- **`write_mutex`**: Mutex que garante exclusividade durante a escrita.
- **`mutex`**: Mutex para proteger o acesso às variáveis de controle.

#### Leitura:
1. Antes de ler, a thread verifica se há solicitações de escrita pendentes (`write_request > 0`). Se houver, a thread de leitura espera até que as escritas terminem.
2. Se não houver escritores pendentes, o contador de leitores ativos (`read_count`) é incrementado.
3. Se for o primeiro leitor, a thread bloqueia o mutex de escrita para garantir que nenhum escritor inicie.
4. Após a leitura, o contador de leitores é decrementado. Se for o último leitor, o mutex de escrita é liberado, permitindo que escritores iniciem.

#### Escrita:
1. Antes de escrever, a thread de escrita incrementa o contador de solicitações de escrita (`write_request`) para indicar que uma operação de escrita está pendente.
2. A thread bloqueia o mutex de escrita, garantindo que nenhuma leitura ou outra escrita ocorra durante a operação.
3. Após concluir a escrita, o contador de solicitações de escrita é decrementado e, se não houver mais escritores, as threads de leitura são liberadas.

## Resultados

Ao rodar o código com diferentes quantidades de threads, os seguintes resultados foram obtidos:

```bash
./main_writer_priority 2
Thread 1: in=50309 out=50272 read=4899419
Thread 0: in=50251 out=49801 read=4899948
Tempo: 3.425763
```

```bash
./main_writer_priority 4
Thread 0: in=25155 out=24973 read=2449872
Thread 2: in=25148 out=25074 read=2449778
Thread 1: in=25157 out=24979 read=2449864
Thread 3: in=25100 out=25047 read=2449853
Tempo: 10.137328
```

```bash
./main_writer_priority 8
Thread 4: in=12548 out=12362 read=1225090
Thread 0: in=12787 out=12460 read=1224753
Thread 1: in=12663 out=12638 read=1224699
Thread 7: in=12530 out=12620 read=1224850
Thread 5: in=12525 out=12412 read=1225063
Thread 6: in=12573 out=12498 read=1224929
Thread 2: in=12492 out=12423 read=1225085
Thread 3: in=12442 out=12660 read=1224898
Tempo: 18.834568
```

```bash
./main_writer_priority 16
Thread 0: in=6310 out=6126 read=612564
Thread 4: in=6292 out=6350 read=612358
Thread 5: in=6350 out=6199 read=612451
Thread 6: in=6308 out=6225 read=612467
Thread 12: in=6347 out=6165 read=612488
Thread 11: in=6216 out=6251 read=612533
Thread 8: in=6416 out=6263 read=612321
Thread 9: in=6051 out=6344 read=612605
Thread 13: in=6470 out=6253 read=612277
Thread 2: in=6372 out=6246 read=612382
Thread 10: in=6217 out=6347 read=612436
Thread 1: in=6195 out=6156 read=612649
Thread 3: in=6191 out=6244 read=612565
Thread 14: in=6268 out=6346 read=612386
Thread 15: in=6261 out=6354 read=612385
Thread 7: in=6296 out=6204 read=612500
Tempo: 28.981587
```

### Análise dos Resultados

Os resultados obtidos ao rodar o código com diferentes quantidades de threads mostram que:

- À medida que o número de threads aumenta, o tempo de execução também aumenta. Isso é esperado, pois, com mais threads, há maior competição por recursos (mutexes) e mais contenção entre as threads para acessar a lista compartilhada.
- A prioridade de escrita foi contemplada. Mesmo com muitas leituras, as threads de escrita foram sempre executadas conforme solicitado. Isso pode ser verificado pelo fato de que a quantidade de inserções e remoções realizadas foi consistente entre as diferentes execuções, independentemente do número de threads.
- Embora o paralelismo tenha sido alcançado, a contenção devido à sincronização das threads resulta em um tempo de execução maior conforme o número de threads aumenta.

## Conclusão

A implementação de **rwlock com prioridade para escrita** foi bem-sucedida, garantindo que as operações de escrita fossem executadas antes das leituras quando necessário, respeitando a prioridade de escrita. O desempenho foi testado com diferentes números de threads, mostrando que, apesar de a contenção aumentar o tempo de execução, a lógica de prioridade foi corretamente aplicada.
