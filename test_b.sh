./bin/data_gen 100000 1000 10 0 1000  > /dev/null
mkdir -p tests/testb
for i in $(seq 1 10); do
    ./bin/knn_main "train.bin" "test.bin" 5 $i > "tests/testb/testb_nthread_$i.txt"
done
