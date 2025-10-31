#include "heap.h"
#include <stdlib.h>
#include <pthread.h>

void heap_init(Heap *h, int length) {
    h->data = (HeapElem*) malloc(sizeof(HeapElem) * length);
    h->n_elem = 0;
    h->length = length;
    pthread_mutex_init(&h->mutex, NULL);
}

void heap_subir(Heap *h, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->data[pai].dist >= h->data[i].dist) break;
        HeapElem tmp = h->data[pai];
        h->data[pai] = h->data[i];
        h->data[i] = tmp;
        i = pai;
    }
}

void heap_descer(Heap *h, int i) {
    int esq, dir, maior;
    while ((esq = 2 * i + 1) < h->n_elem) {
        dir = esq + 1;
        maior = i;
        if (h->data[esq].dist > h->data[maior].dist) maior = esq;
        if (dir < h->n_elem && h->data[dir].dist > h->data[maior].dist) maior = dir;
        if (maior == i) break;
        HeapElem tmp = h->data[i];
        h->data[i] = h->data[maior];
        h->data[maior] = tmp;
        i = maior;
    }
}

void heap_inserir(Heap *h, double dist, int id) {
    if (h->n_elem < h->length) {
        h->data[h->n_elem++] = (HeapElem){dist, id};
        heap_subir(h, h->n_elem - 1);
    } else if (dist < h->data[0].dist) {
        h->data[0] = (HeapElem){dist, id};
        heap_descer(h, 0);
    }
}

void heap_libera(Heap *h) {
    free(h->data);
    pthread_mutex_destroy(&h->mutex);
}
