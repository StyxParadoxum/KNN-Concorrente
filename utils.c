#include "utils.h"
#include <math.h>

double distancia(const Ponto *a, const Ponto *b, int dim) {
  double sum_of_squares = 0;
  for (int i = 0; i < dim; i++) {
    sum_of_squares += pow(a->features[i] - b->features[i], 2);
  }
  return (double) sqrt(sum_of_squares);
}
