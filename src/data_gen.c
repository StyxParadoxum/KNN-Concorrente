#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para gerar um número float aleatório no intervalo [min, max]
float random_in_range(float min, float max) {
    return min + ((float) rand() / RAND_MAX) * (max - min);
}

// Função para gerar um dataset binário com pontos aleatórios dados pela random_in_range
void generate_dataset(const char *filename, int points, int dimensions, float min, float max) {
    
    FILE *file = fopen(filename, "wb");
    
    if (!file) {
        perror("Erro ao criar arquivo");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < points; i++) {
        for (int j = 0; j < dimensions; j++) {
            float value = random_in_range(min, max);
            fwrite(&value, sizeof(float), 1, file);
        }
    }

    fclose(file);
}

// Função para imprimir pontos do dataset
void print_dataset(const char *filename, int points, int dimensions, int max_print) {
    
    FILE *file = fopen(filename, "rb");
    
    if (!file) {
        perror("Erro ao abrir arquivo para leitura");
        return;
    }

    printf("\n--- Conteúdo de %s ---\n", filename);
    
    for (int i = 0; i < points && i < max_print; i++) {
        printf("Ponto %d: ", i + 1);
        for (int j = 0; j < dimensions; j++) {
            float value;
            fread(&value, sizeof(float), 1, file);
            printf("%.2f ", value);
        }
        printf("\n");
    }

    if (points >= max_print) {
        printf("--===(%d pontos no total)===--\n", points);
    }

    fclose(file);
}

// Função principal para testar a geração de datasets de treino e teste e printá-los
int main(int argc, char *argv[]) {
    
    if (argc != 6) {
        fprintf(stderr, "Uso: %s <N_treino> <M_teste> <D_dimensao> <min> <max>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s 1000 200 4 0 100\n", argv[0]);
        return -1;
    }

    int N = atoi(argv[1]);       // pontos de treino
    int M = atoi(argv[2]);       // pontos de teste
    int D = atoi(argv[3]);       // dimensões
    float min = atof(argv[4]);   // valor mínimo
    float max = atof(argv[5]);   // valor máximo

    srand(time(NULL));

    printf("Gerando %d pontos de treino e %d de teste (%d dimensões) no intervalo [%.2f, %.2f]\n",
           N, M, D, min, max);

    generate_dataset("train.bin", N, D, min, max);
    generate_dataset("test.bin", M, D, min, max);

    printf("\nArquivos 'train.bin' e 'test.bin' gerados com sucesso!\n");

    // Impressão dos primeiros pontos
    print_dataset("train.bin", N, D, N);
    print_dataset("test.bin", M, D, M);

    return 0;
}
