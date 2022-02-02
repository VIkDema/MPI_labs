from random import randint
import sys
res=""
var = int(sys.argv[1])
for i in range(0,var):
    res+=str(randint(1, 350))+" "
file = open('data.txt','w')
file.write(res)