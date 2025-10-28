# KNN-Concorrente

Implementação concorrente do algoritmo K-Nearest Neighbors (KNN) em C usando threads POSIX.

## Descrição

Este projeto implementa uma versão paralela do algoritmo KNN que calcula os K vizinhos mais próximos para cada ponto de teste em relação a um conjunto de treino. A implementação utiliza threads para paralelizar o processamento, melhorando significativamente o desempenho para datasets grandes.

## Arquitetura

O projeto está organizado nos seguintes módulos:

- **main.c**: Módulo principal que coordena a execução
- **heap.h/heap.c**: Implementação de heap de máximo thread-safe para armazenar os K vizinhos mais próximos
- **utils.h/utils.c**: Funções utilitárias incluindo cálculo de distância euclidiana e função worker das threads
- **knn.h**: Definições das estruturas Dataset e Ponto
- **data_gen.c**: Gerador de datasets de teste e treino

### Estruturas principais

```c
typedef struct {
  double *features; // Vetor de características
  int id;          // Identificador do ponto
} Ponto;

typedef struct {
  Ponto *treino;   // Conjunto de treino
  Ponto *teste;    // Conjunto de teste
  int M;           // Número de pontos de teste
  int N;           // Número de pontos de treino
  int D;           // Número de dimensões
  int K;           // Número de vizinhos mais próximos
} Dataset;
```

## Compilação

O projeto inclui um Makefile para facilitar a compilação:

```bash
# Compilar todos os programas
make all

# Compilar apenas o programa principal
make bin/knn_main

# Compilar apenas o gerador de dados
make bin/data_gen
```

## Uso

### 1. Gerar datasets de teste

```bash
# Gera datasets com 1000 pontos de treino e 200 de teste, 4 dimensões, valores entre 0 e 100
make generate_data

# Ou executar diretamente:
./bin/data_gen 1000 200 4 0 100
```

### 2. Executar o algoritmo KNN

```bash
# Executar com os datasets gerados
make run

# Ou executar diretamente:
./bin/knn_main train.bin test.bin 1000 200 4 5
```

Onde os parâmetros são:
- `train.bin`: arquivo binário com dados de treino
- `test.bin`: arquivo binário com dados de teste
- `1000`: número de pontos de treino
- `200`: número de pontos de teste
- `4`: número de dimensões dos pontos
- `5`: número de vizinhos mais próximos (K)

### 3. Teste completo

```bash
# Gera dados e executa o programa automaticamente
make test
```

## Características

### Paralelização

- Utiliza 4 threads por padrão para processamento paralelo
- Cada thread processa uma fatia do conjunto de treino
- Heaps thread-safe com mutexes para evitar condições de corrida

### Estrutura de Dados

- **Heap de máximo**: Mantém os K vizinhos mais próximos para cada ponto de teste
- **Thread safety**: Cada heap possui seu próprio mutex
- **Memória eficiente**: Alocação dinâmica com limpeza adequada

### Métricas de Desempenho

O programa exibe estatísticas detalhadas:
- Tempo de leitura dos dados
- Tempo de processamento paralelo
- Tempo total de execução
- Número de threads utilizadas

## Saída

### Terminal
O programa exibe informações de progresso e estatísticas de execução no terminal.

### Arquivo output.txt
Os resultados são salvos em `output.txt` com o formato:
```
Resultados do KNN (K=5)
==============================

Ponto de teste 0:
K-vizinhos mais próximos:
  ID: 439, Distância: 33.570272
  ID: 244, Distância: 29.351958
  ...
```

## Exemplo de Execução

```bash
$ make test
./bin/data_gen 1000 200 4 0 100
Gerando 1000 pontos de treino e 200 de teste (4 dimensões) no intervalo [0.00, 100.00]
Arquivos 'train.bin' e 'test.bin' gerados com sucesso!
Datasets gerados: train.bin (1000 pontos) e test.bin (200 pontos)

./bin/knn_main train.bin test.bin 1000 200 4 5
=== INICIANDO EXECUÇÃO DO KNN CONCORRENTE ===
Lendo dataset de treino...
Lendo dataset de teste...
Datasets carregados com sucesso!
Treino: 1000 pontos, Teste: 200 pontos, Dimensões: 4, K: 5
Iniciando processamento paralelo com 4 threads...
Processamento paralelo concluído!

=== ESTATÍSTICAS DE EXECUÇÃO ===
Tempo de leitura dos dados: 0.000969 segundos
Tempo de processamento paralelo: 0.012666 segundos
Tempo total de execução: 0.016010 segundos
Número de threads utilizadas: 4
===============================
```

## Limpeza

```bash
# Remove arquivos compilados e dados gerados
make clean
```

## Requisitos

- GCC com suporte a C99
- Biblioteca pthread
- Sistema Unix/Linux

## Comandos Úteis

```bash
make help    # Mostra ajuda com todos os comandos disponíveis
make clean   # Limpa arquivos compilados
make all     # Compila todos os programas
make test    # Execução completa (gerar dados + executar)
```