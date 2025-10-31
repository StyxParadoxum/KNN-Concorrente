# Makefile para o projeto KNN-Concorrente

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -pthread
TARGET = knn_main
SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/heap.c $(SRCDIR)/utils.c
OBJECTS = $(SOURCES:.c=.o)

# Diretório de saída
BINDIR = bin

# Ativar modo de depuração se DEBUG=1 for passado
ifeq ($(DEBUG),1)
    CFLAGS += -DDEBUG -g
endif

# Regra principal
all: $(BINDIR) $(BINDIR)/$(TARGET) $(BINDIR)/data_gen

# Criar diretório bin se não existir
$(BINDIR):
	mkdir -p $(BINDIR)

# Compilar o programa principal
$(BINDIR)/$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) -lm

# Compilar o gerador de dados
$(BINDIR)/data_gen: $(SRCDIR)/data_gen.c
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/data_gen.c -lm

# Gerar dados de exemplo
generate_data: $(BINDIR)/data_gen
	./$(BINDIR)/data_gen 1000 200 4 0 100
	@echo "Datasets gerados: train.bin (1000 pontos) e test.bin (200 pontos)"

# Executar o programa principal com dados de exemplo
run: $(BINDIR)/$(TARGET)
	./$(BINDIR)/$(TARGET) train.bin test.bin 1000 200 4 5

# Executar teste completo (gerar dados + executar)
test: generate_data run

# Limpeza
clean:
	rm -rf $(BINDIR)
	rm -f train.bin test.bin output.txt

# Regras especiais
.PHONY: all clean generate_data run test

# Informações de ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make all           - Compila todos os programas"
	@echo "  make generate_data - Gera datasets de exemplo"
	@echo "  make run          - Executa o programa principal"
	@echo "  make test         - Gera dados e executa o programa"
	@echo "  make clean        - Remove arquivos compilados e dados"
	@echo "  make help         - Mostra esta ajuda"
