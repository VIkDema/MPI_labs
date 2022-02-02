gcc -fopenmp srcOpenMP/main.c -o build/main -Ofast
./build/main $1 "matrices/A.txt" "matrices/B.txt" "matrices/result.txt"