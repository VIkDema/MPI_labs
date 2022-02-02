import sys
import random

def generate_and_write_matrix(info):
    matrix = [ [0]*info[2] for i in range(info[1]) ]
    for i in range(info[1]):
        for j in range(info[2]):
            matrix[i][j] = random.randint(-10, 10)
    f = open(info[0], 'w')
    f.write(""+str(info[1])+" "+str(info[2])+"\n")
    for row in matrix:
        f.write(' '.join(list(map(str, row)))+"\n")

if __name__ == "__main__":
    print('hello,world!')
    if len(sys.argv) != 7:
        print("error")
        exit()
    A = [sys.argv[1], int(sys.argv[2]), int(sys.argv[3])]
    B = [sys.argv[4], int(sys.argv[5]), int(sys.argv[6])]
    if A[2] != B[1]:
        print("The number of columns of the first is not equal to the number of rows of the second")
        exit()
    generate_and_write_matrix(A)
    generate_and_write_matrix(B)