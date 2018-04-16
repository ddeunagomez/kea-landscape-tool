import sys
import random
from collections import defaultdict

def print_error(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


class Network(object):
    def __init__(self,side, degree, alternatives = 1):
        self.edges_ = []
        self.oasis_ = []
        self.side_ = side
        self.degree_ = degree
        self.node2edges_ = defaultdict(list)
        for row in range(side):
            for column in range(side):
                cell = row * side + column
                if column < side - 1:
                    self.addEdge(cell, cell + 1, makeRandomConductances(alternatives))
                if row < side - 1:
                    self.addEdge(cell, cell + side, makeRandomConductances(alternatives))
                    if degree == 8:
                        self.addEdge(cell, cell + side + 1, makeRandomConductances(alternatives))
                if degree == 8 and row > 0  and column < side - 1 :
                    self.addEdge(cell, cell - side + 1, makeRandomConductances(alternatives))

    def adjacent(self, position):
        a = []
        row = int(position/self.side_)
        column = position % self.side_
        if column < side - 1:
            a.append(position + 1)
        if row < self.side_ - 1:
            a.append(position + side)
            if degree == 8:
                a.append(position + side + 1)
        if degree == 8 and row > 0  and column < side - 1 :
            a.append(cell - side + 1)
        return a

    def addEdge(self,a,b,conductances):
        self.edges_.append((a,b,conductances))
        self.node2edges_[a].append(len(self.edges_) - 1)
        self.node2edges_[b].append(len(self.edges_) - 1)

    def makeOasis(self, number):
        self.oasis_ = []
        for _ in range(0,number):
            p = random.randint(0,self.side_*self.side_-1)
            self.oasis_.append(p)
            for other in self.adjacent(p):
                self.oasis_.append(other)
        for p in self.oasis_:
            for e in self.node2edges_[p]:
                t = self.edges_[e]
                self.edges_[e] = (t[0],t[1],[1]+t[2][1:])
                

    def toDot(self):
        s = "graph {"

        for a,b,l in self.edges_:
            s += str(a) + "--" + str(b) + " [label="
            for val in l:
                s += str(round(val,4))+"\n"
            s += "]\n"

        s += "}"

        return s

    def toTextLists(self):
        files = [ "" for i in range(len(self.edges_[0][2]))]

        for i in range(len(files)):
            s = ""
            for a,b,l in self.edges_:
                s += str(a) + " " + str(b) + " " + str(round(l[i],4)) + "\n"
            files[i] = s
        return files


def randomConductace(): 
    #return random.uniform(0.01,1.0)
    return (round(random.betavariate(5,80),2)+0.005)%1

def randomImprovementFactor(): 
    return random.uniform(1.5,10.0)

def makeRandomConductances(alternatives):
    conductances = []
    for i in range(alternatives + 1):
        if not conductances:
            conductances.append(randomConductace())
        else:
            conductances.append(1)#conductances[0] * randomImprovementFactor())

    return conductances



if __name__ == "__main__":
    side = int(sys.argv[1])
    degree = int(sys.argv[2])
    alternatives = int(sys.argv[3])
    destination_base = sys.argv[4]

    if degree != 4 and degree != 8:
        print_error("Error. degree must be 4 or 8")
        exit(1)
    if (side < 4):
        print_error("Minimum grid side is 4")
        exit(1)

    n = Network(side,degree,alternatives)
    n.makeOasis(2*side)

    l = n.toTextLists()

    with open(destination_base+".ntw", "w") as file:
        file.write(l[0])

    for i in range(1,len(l)):
        with open(destination_base+"_alt_"+str(i)+".ntw", "w") as file:
            file.write(l[i])

    print("Done")