./bin/data_gen 100000 1000 10 0 1000 > /dev/null
mkdir -p tests/testb
./bin/knn_main "train.bin" "test.bin" 5 $1 "tests/test_b.txt" > "tests/testb/testb_nthread_$1.txt"
