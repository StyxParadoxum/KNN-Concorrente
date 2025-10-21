/**
 * @file utils.h
 * @brief Declarações de funções e estruturas utilitárias do projeto.
 *
 * Este módulo contém funções auxiliares e estruturas de apoio usadas
 * em diferentes partes do programa, incluindo operações matemáticas
 * básicas (como o cálculo de distância euclidiana) e suporte à execução
 * paralela com *threads*.
 */

#ifndef UTILS_H
#define UTILS_H

#include "knn.h"
#include "heap.h"

/**
 * @brief Estrutura de argumentos para funções executadas em threads.
 *
 * @details Estrutura usada para passagem de parâmetros a uma função
 * executada por `pthread_create`. Cada thread processará uma fatia
 * específica do conjunto de dados de treino, delimitada pelos índices
 * `ini` e `fim`.
 *
 * O campo `dataset` aponta para a estrutura que contém os dados
 * necessários para o cálculo das distâncias entre os pontos de treino
 * e teste.
 */
typedef struct {
  Dataset *dataset; /**< Ponteiro para o conjunto de dados principal. */
  Ponto *ini;       /**< Ponto inicial da fatia de dados de treino processada pela thread. */
  Heap ** heaps;    /**< Ponteiro para um vetor de apontadores das heaps dos pontos de teste. */
  int n;            /**< Quantidade de pontos presentes na fatia. */
} ThreadArgs;

/**
 * @brief Função a ser executada pelas threads
 *
 * @details Essa função é responsável por calcular as distâncias e
 * usar as funções definidas em heap.h para conseguir determinar os
 * k vizinhos mais próximos
 *
 * @param args um ponteiro do tipo void para uma estrutura do tipo ThreadArgs
 * @return por ora, nada.
 */
void *thread_worker(void *args);

/**
 * @brief Calcula a distância euclidiana entre dois pontos.
 *
 * @details A distância euclidiana é definida como a raiz quadrada da soma dos
 * quadrados das diferenças entre as coordenadas correspondentes dos pontos:
 * \f[
 * d(a,b) = \sqrt{\sum_{i=1}^{dim} (a_i - b_i)^2}
 * \f]
 *
 * Esta função é amplamente usada em algoritmos de aprendizado de máquina,
 * como o K-Nearest Neighbors (KNN).
 *
 * @param a Ponteiro para o primeiro ponto.
 * @param b Ponteiro para o segundo ponto.
 * @param dim Dimensão dos pontos (número de atributos).
 * @return Distância euclidiana entre os pontos `a` e `b`.
 */
double distancia(const Ponto *a, const Ponto *b, int dim);

#endif // !UTILS_H
