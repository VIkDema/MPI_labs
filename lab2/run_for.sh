#!/bin/bash
clear
mpicc main.c -o main.x
for ((i=0; i<10000; i+=100))
do 
    echo ${i}
    mpirun -np 8 ./main.x ${i}
done
