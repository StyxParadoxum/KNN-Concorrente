/**
 * @file heap.h
 * @brief Interface pública para uma heap de máximo genérica.
 *
 * Este módulo implementa uma estrutura de dados do tipo *heap de máximo*,
 * que organiza elementos de modo que o maior valor (com base em um critério de prioridade)
 * fique sempre na raiz. É utilizada aqui para armazenar pares (distância, id),
 * onde a prioridade é dada pelo valor de `dist`.
 *
 * A heap é útil em diversos algoritmos de ordenação parcial, busca de k-vizinhos mais próximos,
 * filas de prioridade, entre outros.
 */

#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>

/**
 * @brief Representa um elemento armazenado na heap.
 *
 * Cada elemento contém um valor de distância e um identificador associado.
 * A prioridade do elemento na heap é determinada pelo campo `dist`.
 */
typedef struct {
  double dist;  /**< Distância entre o ponto de treino e o ponto consultado. */
  int id;       /**< Identificador ou rótulo do ponto de treino. */
} HeapElem;

/**
 * @brief Estrutura que representa uma heap de máximo.
 *
 * Contém um vetor de elementos (`data`), o número atual de elementos (`n_elem`),
 * e a capacidade máxima (`length`).
 */
typedef struct {
  HeapElem *data; /**< Vetor de elementos armazenados na heap. */
  int n_elem;     /**< Número atual de elementos presentes na heap. */
  int length;     /**< Capacidade máxima da heap. */
} Heap;

/**
 * @brief Inicializa uma heap de máximo.
 *
 * Aloca o espaço necessário para o vetor de elementos e define os parâmetros iniciais.
 *
 * @param h Ponteiro para uma estrutura Heap não inicializada.
 * @param length Tamanho máximo (capacidade) da heap.
 * @return void
 */
void heap_init(Heap *h, int length);

/**
 * @brief Move um elemento para cima na heap (*heapify-up*).
 *
 * @details Reorganiza a heap de modo que a propriedade de heap de máximo
 * seja mantida. Este procedimento é utilizado após a inserção de um novo elemento
 * no final da estrutura.
 *
 * @param h Ponteiro para uma heap previamente inicializada.
 * @param i Índice do elemento a ser elevado. Em chamadas típicas, é `h->n_elem - 1`.
 * @return void
 */
void heap_subir(Heap *h, int i);

/**
 * @brief Move um elemento para baixo na heap (*heapify-down*).
 *
 * @details Reorganiza a heap de modo que o maior elemento permaneça na raiz.
 * Este procedimento é utilizado após a remoção do elemento máximo, realocando
 * o último elemento para a raiz e restabelecendo a propriedade da heap.
 *
 * @param h Ponteiro para uma heap válida.
 * @param i Índice do elemento a ser rebaixado (geralmente 0 após uma remoção).
 * @return void
 */
void heap_descer(Heap *h, int i);

/**
 * @brief Insere um novo elemento na heap.
 *
 * @details Adiciona o elemento ao final do vetor interno e chama `heap_subir`
 * para restaurar a propriedade da heap.
 *
 * @param h Ponteiro para uma heap previamente inicializada e não cheia.
 * @param dist Valor de prioridade (distância) do elemento a ser inserido.
 * @param id Identificador associado ao elemento.
 * @return void
 */
void heap_inserir(Heap *h, double dist, int id);

/**
 * @brief Libera a memória associada à heap.
 *
 * @details Libera o vetor de elementos alocado internamente e zera os campos
 * da estrutura para evitar uso indevido posterior.
 *
 * @param h Ponteiro para uma heap previamente inicializada.
 * @return void
 */
void heap_libera(Heap *h);

#endif // !HEAP_H
