/**
 * @file knn.h
 * @brief Estruturas e funções para representação de elementos
 *        em um dataset
 */

#ifndef KNN_H
#define KNN_H
/**
 * @brief Representação de um ponto rotulado
 */
typedef struct {
  double *features; /**< Vetor de features */
  int id;           /**< rótulo do ponto   */
} Ponto;

/**
* @brief Representação de um dataset de entrada a ser manipulado no programa
*
* @details A estrutura contém campos para arrays de Pontos de teste e treino,
* e outros metadados sobre o conjunto de dados.
*/
typedef struct {
  Ponto * treino;   /*<< conjunto de treino */
  Ponto * teste;    /*<< conjunto de teste */
  int M;            /*<< número de instâncias de teste */
  int N;            /*<< número de instâncias de treino */
  int D;            /*<< número de dimensão dos pontos */
  int K;            /*<< número de vizinhos mais próximo */
} Dataset;

#endif // !KNN_H
