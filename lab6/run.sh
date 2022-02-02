
mpicc srcMPI/main.c  -o build/main.x 
mpirun -np $1 ./build/main.x "matrices/A.txt" "matrices/B.txt" "matrices/result.txt"
