#!/bin/bash
clear
mpicc main.c -o main.x
for ((i=0; i<1000000; i+=100000))
do 
    python script.py ${i}
    mpirun -np 8 ./main.x
done
