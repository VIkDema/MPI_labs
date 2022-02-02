for ((i=10; i<500; i+=10))
do 
    bash generateMatrices.sh ${i}
    mpirun -np $1 ./build/main.x "matrices/A.txt" "matrices/B.txt" "matrices/result.txt"
    ./build/main $1 "matrices/A.txt" "matrices/B.txt" "matrices/result.txt"
    echo ${i}
done