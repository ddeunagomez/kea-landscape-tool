import sys
import random

deg = int(sys.argv[1])
if deg != 4 and deg != 8:
    print("Error. Degree must be 4 or 8")
    exit(1)

sd = int(sys.argv[2])
if (sd < 4):
    print("Minimum grid side is 4")
    exit(1)

DOT = False

if DOT:
    print "graph {"
MID = " -- " if DOT else "\t"

randfun = lambda : round(random.uniform(0.01,1.0),4)

for r in range(0,sd):
    for c in range(0,sd):
        cell = r*sd + c
        if c < sd - 1:
            cond = "\t"+("" if DOT else str(randfun()))
            print("\t"+str(cell)+MID+str(cell+1)+cond)
        if r < sd - 1:
            cond = "\t"+("" if DOT else str(randfun()))
            print("\t"+str(cell)+MID+str(cell+sd)+cond)
            if deg == 8 and c < sd - 1:
                cond = "\t"+("" if DOT else str(randfun()))
                print("\t"+str(cell)+MID+str(cell+sd+1)+cond)
        if deg == 8 and r > 0 and c < sd - 1:
            cond = "\t"+("" if DOT else str(randfun()))
            print("\t"+str(cell)+MID+str(cell-sd+1)+cond)

if DOT:
    print "}"


