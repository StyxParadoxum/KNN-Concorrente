./bin/data_gen 1000 100 2 0 1000 > /dev/null
mkdir -p tests/testa/
./bin/knn_main "train.bin" "test.bin" 3 $1 "tests/test_a.txt" > "tests/testa/testa_nthread_$1.txt"
