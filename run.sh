#!/bin/bash

# Verificar se foi passado um argumento
if [ $# -eq 0 ]; then
    echo "Uso: $0 <script_de_teste>"
    echo "Exemplos:"
    echo "  $0 test_a.sh    # Executa cenário A"
    echo "  $0 test_b.sh    # Executa cenário B"
    echo "  $0 test_c.sh    # Executa cenário C"
    exit 1
fi

# Verificar se o arquivo existe
if [ ! -f "$1" ]; then
    echo "Erro: Arquivo '$1' não encontrado!"
    exit 1
fi

# Obter o nome do script sem extensão para identificação
script_name=$(basename "$1" .sh)

echo "=== Executando $script_name com 1 a 10 threads ==="
echo "Script: $1"
echo ""

# Tornar o script executável se não estiver
chmod +x "$1"

# Executar o script com 1 a 10 threads
for threads in $(seq 1 10); do
    echo "--- Executando com $threads thread(s) ---"
    
    # Executar o script passando o número de threads como argumento
    ./"$1" $threads
    
    if [ $? -eq 0 ]; then
        echo "✅ Concluído com $threads thread(s)"
    else
        echo "❌ Erro ao executar com $threads thread(s)"
    fi
    echo ""
done

echo "=== Teste $script_name concluído ==="

# Mostrar onde os resultados foram salvos baseado no script
case "$script_name" in
    "test_a")
        echo "Resultados salvos em: tests/testa/"
        ;;
    "test_b")
        echo "Resultados salvos em: tests/testb/"
        ;;
    "test_c")
        echo "Resultados salvos em: tests/testc/"
        ;;
    *)
        echo "Verifique o diretório tests/ para os resultados"
        ;;
esac