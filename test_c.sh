./bin/data_gen 1000000 10000 100 0 1000  > /dev/null
mkdir -p tests/testc
for i in $(seq 1 10); do
    ./bin/knn_main "train.bin" "test.bin" 5 $i > "tests/testc/testc_nthread_$i.txt"
done
