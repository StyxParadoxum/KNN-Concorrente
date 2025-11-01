./bin/data_gen 1000000 10000 50 0 1000 > /dev/null
mkdir -p tests/testc
./bin/knn_main "train.bin" "test.bin" 10 $1 "tests/test_c.txt" > "tests/testc/testc_nthread_$1.txt"
