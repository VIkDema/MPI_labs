#!/bin/bash
clear
mpicc main.c -o main.x
for ((i=1; i<1000000; i+=2000))
do 
    mpirun -np 9 ./main.x ${i}
    echo ${i}
done
