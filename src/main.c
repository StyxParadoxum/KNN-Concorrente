/**
 * @file main.c
 * @brief Módulo principal do programa KNN concorrente
 *
 * Este módulo coordena a execução do algoritmo K-Nearest Neighbors de forma
 * paralela, integrando os módulos heap, utils e knn para processar datasets de
 * treino e teste.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "heap.h"
#include "knn.h"
#include "utils.h"

/**
 * @brief Lê um dataset binário de um arquivo
 *
 * @param filename Nome do arquivo binário
 * @param pontos Array de pontos a ser preenchido
 * @param n_pontos Número de pontos no dataset
 * @param dimensoes Número de dimensões de cada ponto
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int ler_pontos(FILE *file, Ponto *pontos, int *n_pontos, int *dimensoes) {
  for (int i = 0; i < *n_pontos; i++) {
    pontos[i].features = (double*) malloc(*dimensoes * sizeof(double));
    if (!pontos[i].features) {
      fprintf(stderr, "Erro de alocação de memória para features do ponto %d\n",
              i);
      return -1;
    }

    pontos[i].id = i; // Atribui ID sequencial

    // Lê as features do ponto (assumindo que o arquivo contém doubles)
    for (int j = 0; j < *dimensoes; j++) {
      double value;
      if (fread(&value, sizeof(double), 1, file) != 1) {
        fprintf(stderr, "Erro ao ler feature %d do ponto %d\n", j, i);
        return -1;
      }
      pontos[i].features[j] = value;
    }
  }

  return 0;
}

int ler_metadados(FILE *file, int *n_pontos, int *dim) {
  if (fread(n_pontos, sizeof(int), 1, file) != 1) {
    fprintf(stderr, "Erro na leitura na quantidade de pontos do arquivo\n");
    return -1;
  }
  if (fread(dim, sizeof(int), 1, file) != 1) {
    fprintf(stderr, "Erro na leitura na dimensão dos pontos do arquivo\n");
    return -1;
  }
  return 0;
}

/**
 * @brief Inicializa o dataset com dados de treino e teste
 */
int inicializar_dataset(Dataset *dataset, const char *arquivo_treino,
                        const char *arquivo_teste, int K) {

  dataset->K = K;

  FILE *file_treino = fopen(arquivo_treino, "rb");
  FILE *file_teste = fopen(arquivo_teste, "rb");

  if (!file_treino) {
    fprintf(stderr, "Erro ao abrir arquivo de treino: %s\n", arquivo_treino);
    return -1;
  }
  if (!file_teste) {
    fprintf(stderr, "Erro ao abrir arquivo de teste: %s\n", arquivo_teste);
    fclose(file_treino);
    return -1;
  }

  // Lê metadados dos arquivos
  int dim_treino, dim_teste;

  if (ler_metadados(file_treino, &dataset->N, &dim_treino) != 0) {
    fprintf(stderr, "Falha ao ler metadados do arquivo de treino\n");
    fclose(file_treino);
    fclose(file_teste);
    return -1;
  }

  if (ler_metadados(file_teste, &dataset->M, &dim_teste) != 0) {
    fprintf(stderr, "Falha ao ler metadados do arquivo de teste\n");
    fclose(file_treino);
    fclose(file_teste);
    return -1;
  }

  // Verifica se as dimensões são compatíveis
  if (dim_treino != dim_teste) {
    fprintf(stderr, "Erro: Dimensões incompatíveis - treino: %d, teste: %d\n",
            dim_treino, dim_teste);
    fclose(file_treino);
    fclose(file_teste);
    return -1;
  }

  dataset->D = dim_treino;

  // Valida K
  if (K <= 0 || K > dataset->N) {
    fprintf(stderr, "Erro: K deve estar entre 1 e %d (número de pontos de treino)\n", dataset->N);
    fclose(file_treino);
    fclose(file_teste);
    return -1;
  }

  // Aloca memória para os arrays de pontos
  dataset->treino = (Ponto*) malloc(dataset->N * sizeof(Ponto));
  dataset->teste = (Ponto*) malloc(dataset->M * sizeof(Ponto));

  if (!dataset->treino || !dataset->teste) {
    fprintf(stderr, "Erro de alocação de memória para datasets\n");
    fclose(file_treino);
    fclose(file_teste);
    return -1;
  }

  // Lê os datasets dos arquivos
  printf("Lendo dataset de treino...\n");
  if (ler_pontos(file_treino, dataset->treino, &dataset->N, &dataset->D) != 0) {
    fclose(file_treino);
    fclose(file_teste);
    return -1;
  }

  printf("Lendo dataset de teste...\n");
  if (ler_pontos(file_teste, dataset->teste, &dataset->M, &dataset->D) != 0) {
    fclose(file_treino);
    fclose(file_teste);
    return -1;
  }

  fclose(file_treino);
  fclose(file_teste);

  printf("Datasets carregados com sucesso!\n");
  printf("Treino: %d pontos, Teste: %d pontos, Dimensões: %d, K: %d\n",
         dataset->N, dataset->M, dataset->D, dataset->K);

  return 0;
}

/**
 * @brief Inicializa as heaps para cada ponto de teste
 *
 * @param heaps Array de heaps
 * @param M Número de pontos de teste (número de heaps)
 * @param K Capacidade de cada heap
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int inicializar_heaps(Heap *heaps, int M, int K) {
  for (int i = 0; i < M; i++) {
    heap_init(&heaps[i], K);
  }
  return 0;
}

/**
 * @brief Libera a memória das heaps
 *
 * @param heaps Array de heaps
 * @param M Número de heaps
 */
void liberar_heaps(Heap *heaps, int M) {
  for (int i = 0; i < M; i++) {
    heap_libera(&heaps[i]);
  }
}

/**
 * @brief Libera a memória do dataset
 *
 * @param dataset Ponteiro para a estrutura Dataset
 */
void liberar_dataset(Dataset *dataset) {
  if (dataset->treino) {
    for (int i = 0; i < dataset->N; i++) {
      free(dataset->treino[i].features);
    }
    free(dataset->treino);
  }

  if (dataset->teste) {
    for (int i = 0; i < dataset->M; i++) {
      free(dataset->teste[i].features);
    }
    free(dataset->teste);
  }
}

/**
 * @brief Salva os resultados em um arquivo
 *
 * @param heaps Array de heaps com os resultados
 * @param M Número de pontos de teste
 * @param K Número de vizinhos mais próximos
 * @param filename Nome do arquivo de saída
 */
void salvar_resultados(Heap *heaps, int M, int K, const char *filename) {
  FILE *file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "Erro ao criar arquivo de saída %s\n", filename);
    return;
  }

  fprintf(file, "Resultados do KNN (K=%d)\n", K);
  fprintf(file, "==============================\n\n");

  for (int i = 0; i < M; i++) {
    fprintf(file, "Ponto de teste %d:\n", i);
    fprintf(file, "K-vizinhos mais próximos:\n");

    for (int j = 0; j < heaps[i].n_elem; j++) {
      fprintf(file, "  ID: %d, Distância: %.6f\n", heaps[i].data[j].id,
              heaps[i].data[j].dist);
    }
    fprintf(file, "\n");
  }

  fclose(file);
  printf("Resultados salvos em %s\n", filename);
}

/**
 * @brief Exibe as estatísticas de execução
 *
 * @param tempo_leitura Tempo gasto na leitura dos dados (em segundos)
 * @param tempo_processamento Tempo gasto no processamento paralelo (em
 * segundos)
 * @param tempo_total Tempo total de execução (em segundos)
 * @param num_threads Número de threads utilizadas
 */
void exibir_estatisticas(double tempo_leitura, double tempo_processamento,
                         double tempo_total, int num_threads) {
  printf("\n=== ESTATÍSTICAS DE EXECUÇÃO ===\n");
  printf("Tempo de leitura dos dados: %.6f segundos\n", tempo_leitura);
  printf("Tempo de processamento paralelo: %.6f segundos\n",
         tempo_processamento);
  printf("Tempo total de execução: %.6f segundos\n", tempo_total);
  printf("Número de threads utilizadas: %d\n", num_threads);
  printf("===============================\n");
}

/**
 * @brief Calcula a diferença de tempo entre dois timevals
 *
 * @param start Tempo inicial
 * @param end Tempo final
 * @return Diferença em segundos
 */
double calcular_tempo(struct timeval start, struct timeval end) {
  return (end.tv_sec - start.tv_sec) +
         (end.tv_usec - start.tv_usec) / 1000000.0;
}

/**
 * @brief Função para debug - verifica algumas distâncias manualmente
 */
void debug_distancias(Dataset *dataset) {
  printf("\n=== DEBUG - Verificação de Distâncias ===\n");

  if (dataset->M > 0 && dataset->N > 0) {
    printf("Ponto de teste 0: [");
    for (int j = 0; j < dataset->D; j++) {
      printf("%.2f", dataset->teste[0].features[j]);
      if (j < dataset->D - 1) printf(", ");
    }
    printf("]\n\n");

    printf("Primeiras distâncias calculadas:\n");
    for (int i = 0; i < (dataset->N < 5 ? dataset->N : 5); i++) {
      printf("  Para treino %d [", i);
      for (int j = 0; j < dataset->D; j++) {
        printf("%.2f", dataset->treino[i].features[j]);
        if (j < dataset->D - 1) printf(", ");
      }
      double dist = distancia(&dataset->teste[0], &dataset->treino[i], dataset->D);
      printf("]: %.6f\n", dist);
    }
  }
  printf("========================================\n\n");
}

/**
 * @brief Debug completo do algoritmo KNN
 */
void debug_completo(Dataset *dataset, Heap *heaps) {
  printf("\n=== DEBUG COMPLETO DO KNN ===\n");

  for (int test_idx = 0; test_idx < (dataset->M < 2 ? dataset->M : 2); test_idx++) {
    printf("\nPonto de teste %d: [", test_idx);
    for (int j = 0; j < dataset->D; j++) {
      printf("%.2f", dataset->teste[test_idx].features[j]);
      if (j < dataset->D - 1) printf(", ");
    }
    printf("]\n");

    printf("Distâncias calculadas:\n");
    for (int i = 0; i < dataset->N; i++) {
      double dist = distancia(&dataset->teste[test_idx], &dataset->treino[i], dataset->D);
      printf("  Treino %d: %.6f\n", i, dist);
    }

    // printf("Heap resultante:\n");
    // for (int j = 0; j < heaps[test_idx].n_elem; j++) {
    //   printf("  ID: %d, Distância: %.6f\n", heaps[test_idx].data[j].id, heaps[test_idx].data[j].dist);
    // }
  }
  printf("============================\n\n");
}

/**
 * @brief Função principal
 */
int main(int argc, char *argv[]) {
  if (argc < 5) {
    fprintf(stderr, "Uso: %s <arquivo_treino> <arquivo_teste> <K> <N_THREADS>\n", argv[0]);
    fprintf(stderr, "  arquivo_treino: arquivo binário com dados de treino\n");
    fprintf(stderr, "  arquivo_teste: arquivo binário com dados de teste\n");
    fprintf(stderr, "  K: número de vizinhos mais próximos\n");
    fprintf(stderr, "  N_THREADS: número de threads a serem usadas\n");
    fprintf(stderr, "Exemplo: %s train.bin test.bin 3 4\n", argv[0]);
    return 1;
  }

  // Parse dos argumentos
  const char *arquivo_treino = argv[1];
  const char *arquivo_teste = argv[2];
  int K = atoi(argv[3]);
  int num_threads = atoi(argv[4]);

  // Validação básica dos parâmetros
  if (K <= 0) {
    fprintf(stderr, "Erro: K deve ser positivo\n");
    return 1;
  }
  if (num_threads <= 0) {
    fprintf(stderr, "Erro: Número de threads deve ser positivo\n");
    return 1;
  }

  // Variáveis para medição de tempo
  struct timeval inicio_total, fim_total, inicio_leitura, fim_leitura;
  struct timeval inicio_processamento, fim_processamento;

  gettimeofday(&inicio_total, NULL);

  printf("=== INICIANDO EXECUÇÃO DO KNN CONCORRENTE ===\n");

  // 1. LEITURA E INICIALIZAÇÃO DOS DADOS
  gettimeofday(&inicio_leitura, NULL);

  Dataset dataset;
  if (inicializar_dataset(&dataset, arquivo_treino, arquivo_teste, K) != 0) {
    fprintf(stderr, "Erro na inicialização do dataset\n");
    return 1;
  }

  int N = dataset.N;
  int M = dataset.M;

  // Inicializar heaps para cada ponto de teste
  Heap *heaps = (Heap*) malloc(M * sizeof(Heap));
  if (!heaps) {
    fprintf(stderr, "Erro de alocação de memória para heaps\n");
    liberar_dataset(&dataset);
    return 1;
  }

  if (inicializar_heaps(heaps, M, K) != 0) {
    fprintf(stderr, "Erro na inicialização das heaps\n");
    free(heaps);
    liberar_dataset(&dataset);
    return 1;
  }

  gettimeofday(&fim_leitura, NULL);

  // Debug das primeiras distâncias
#ifdef DEBUG
  debug_completo(&dataset, heaps);
#endif
  // 2. CONFIGURAÇÃO DA EXECUÇÃO PARALELA
  gettimeofday(&inicio_processamento, NULL);

  // Determina o número de threads (pode ser configurável)
  // int num_threads = 4; // Valor padrão, pode ser passado como parâmetro
  // if (num_threads > N) {
  //   num_threads = N; // Não faz sentido ter mais threads que pontos de treino
  // }

  printf("Iniciando processamento paralelo com %d threads...\n", num_threads);

  // Criar arrays para threads e argumentos
  pthread_t *threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
  ThreadArgs *args = (ThreadArgs*) malloc(num_threads * sizeof(ThreadArgs));

  if (!threads || !args) {
    fprintf(stderr, "Erro de alocação de memória para threads\n");
    liberar_heaps(heaps, M);
    free(heaps);
    liberar_dataset(&dataset);
    return 1;
  }

  // Dividir o trabalho entre as threads
  int pontos_por_thread = N / num_threads;
  int pontos_restantes = N % num_threads;

  for (int i = 0; i < num_threads; i++) {
    args[i].dataset = &dataset;
    args[i].heaps = heaps;

    // Calcular o início da fatia para esta thread
    int inicio_fatia = i * pontos_por_thread;
    args[i].ini = &dataset.treino[inicio_fatia];

    // Calcular o número de pontos desta fatia
    args[i].n = pontos_por_thread;
    if (i == num_threads - 1) {
      args[i].n += pontos_restantes; // A última thread pega os pontos restantes
    }

    // Criar a thread
    if (pthread_create(&threads[i], NULL, thread_worker, &args[i]) != 0) {
      fprintf(stderr, "Erro ao criar thread %d\n", i);
      // Cleanup das threads já criadas
      for (int j = 0; j < i; j++) {
        pthread_join(threads[j], NULL);
      }
      free(threads);
      free(args);
      liberar_heaps(heaps, M);
      free(heaps);
      liberar_dataset(&dataset);
      return 1;
    }
  }

  // 3. AGUARDAR CONCLUSÃO DE TODAS AS THREADS
  for (int i = 0; i < num_threads; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
      fprintf(stderr, "Erro ao aguardar thread %d\n", i);
    }
  }

  gettimeofday(&fim_processamento, NULL);

  printf("Processamento paralelo concluído!\n");

  // 4. CLASSIFICAÇÃO E SAÍDA
  printf("Salvando resultados...\n");
  char *filename = argc == 6 ? argv[5] : "output.txt";
  salvar_resultados(heaps, M, K, filename);

  // Exibir alguns resultados no terminal para verificação
  printf("\nPrimeiros resultados (verificação):\n");
  for (int i = 0; i < (M < 3 ? M : 3); i++) {
    printf("Ponto de teste %d - K-vizinhos:\n", i);
    for (int j = 0; j < heaps[i].n_elem; j++) {
      printf("  ID: %d, Distância: %.6f\n", heaps[i].data[j].id,
             heaps[i].data[j].dist);
    }
  }

  gettimeofday(&fim_total, NULL);

  // 5. EXIBIR ESTATÍSTICAS DE EXECUÇÃO
  double tempo_leitura = calcular_tempo(inicio_leitura, fim_leitura);
  double tempo_processamento =
      calcular_tempo(inicio_processamento, fim_processamento);
  double tempo_total = calcular_tempo(inicio_total, fim_total);

  exibir_estatisticas(tempo_leitura, tempo_processamento, tempo_total,
                      num_threads);

  // 6. LIBERAÇÃO DE MEMÓRIA
  free(threads);
  free(args);
  liberar_heaps(heaps, M);
  free(heaps);
  liberar_dataset(&dataset);

  printf("\n=== EXECUÇÃO CONCLUÍDA COM SUCESSO ===\n");
  return 0;
}
