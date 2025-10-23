#include <math.h>
#include <pthread.h>

#include "utils.h"
#include "heap.h"

double distancia(const Ponto *a, const Ponto *b, int dim) {
  double sum_of_squares = 0;
  for (int i = 0; i < dim; i++) {
    sum_of_squares += pow(a->features[i] - b->features[i], 2);
  }
  return (double) sqrt(sum_of_squares);
}

void *thread_worker(void *args) {
  double dist;
  Heap *p_heap;

  ThreadArgs *arg = (ThreadArgs*) args;
  int dim = arg->dataset->D;
  Ponto *p_ponto_treino = arg->ini;
  Ponto *p_ponto_teste = arg->dataset->teste;

  for (int i = 0; i < arg->n; i++) {
    for (int j = 0; j < arg->dataset->M; j++) {
      // computa a distância
      dist = distancia(p_ponto_treino, p_ponto_teste, dim);
      // obtém o ponteiro para a heap do ponto de teste
      p_heap = arg->heaps[j];
      // insere na heap
      pthread_mutex_lock(&p_heap->mutex);
      heap_inserir(p_heap, dist, p_ponto_treino->id);
      pthread_mutex_unlock(&p_heap->mutex);
      // atualiza o ponto de teste
      p_ponto_teste += dim;
    }
    // atualiza o ponto de treino
    p_ponto_treino += dim;
  }
  pthread_exit(NULL);
}
