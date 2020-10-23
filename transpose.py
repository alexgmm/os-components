import numpy as np
from os import listdir
from os.path import isfile, join

def generateTransposedInstances(mypath):
    filenames = [f for f in listdir(mypath) if isfile(join(mypath, f))]

    for filename in filenames:
        header, t = None, None
        with open(mypath + filename, mode="r") as f:
            rows = [r.split() for r in f]
            header, rows = rows[0], rows[1:]
            header[0], header[1] = header[1], header[0]
            t = np.matrix(rows)
            t = t.transpose().tolist()

        with open(mypath + "transposed_" + filename, mode="w") as f:
            print("transposing " + filename)
            f.writelines(' '.join(header) + '\n')
            for line in t:
                if type(line[0]) == list:
                    line = line[0] 
                print(line)
                f.writelines(' '.join(line) + '\n')

for p in ["inst/brucker/","inst/gueret-prins/"]:
    generateTransposedInstances(p)