./bin/data_gen 1000 100 10 0 1000  > /dev/null
mkdir -p tests/testa
for i in $(seq 1 10); do
    ./bin/knn_main "train.bin" "test.bin" 5 $i > "tests/testa/testa_nthread_$i.txt"
done