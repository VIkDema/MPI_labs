#!/bin/bash
clear
mpicc main.c -o main.x
for ((i=8; i<11; i++))
do 
    echo "${i}"
    for ((j=0; j<3; j++))
        do 
            mpirun -np ${i} ./main.x
            echo ""
        done
    
done
